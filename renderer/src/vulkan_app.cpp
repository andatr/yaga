#include "precompiled.h"
#include "vulkan_app.h"
#include "device.h"
#include "extensions.h"
#include "material.h"
#include "swapchain.h"
#include "asset/material.h"
#include "asset/serializer.h"
#include "asset/texture.h"

namespace yaga
{
  ApplicationImpl::InitGLFW ApplicationImpl::initGLFW_;

namespace
{

constexpr size_t MAX_FRAMES = 2;

// const char* for compatibility with Valukan API
// -------------------------------------------------------------------------------------------------------------------------
std::vector<const char*> getExtensions()
{
  uint32_t count;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);
  if (!validationLayers.empty()) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  std::sort(extensions.begin(), extensions.end());
  return extensions;
}

// -------------------------------------------------------------------------------------------------------------------------
VKAPI_ATTR VkBool32 VKAPI_CALL vulkanLog(
  VkDebugUtilsMessageSeverityFlagBitsEXT severity,
  VkDebugUtilsMessageTypeFlagsEXT /*type*/,
  const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
  void* /*userData*/)
{
  if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    LOG(error) << "validation: " << callbackData->pMessage;
    return VK_FALSE;
  } 
  if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    LOG(warning) << "validation: " << callbackData->pMessage;
    return VK_FALSE;
  }
  if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    LOG(info) << "validation: " << callbackData->pMessage;
    return VK_FALSE;
  }
  if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    LOG(debug) << "validation: " << callbackData->pMessage;
    return VK_FALSE;
  }
  LOG(trace) << "validation: " << callbackData->pMessage;
  return VK_FALSE;
}

// -------------------------------------------------------------------------------------------------------------------------
void setDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info)
{
  info = {};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.messageSeverity =
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType =
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pfnUserCallback = vulkanLog;
}

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
ApplicationPtr createApplication()
{
  return std::make_unique<ApplicationImpl>();
}

// -------------------------------------------------------------------------------------------------------------------------
ApplicationImpl::InitGLFW::InitGLFW()
{
  glfwInit();
}

// -------------------------------------------------------------------------------------------------------------------------
ApplicationImpl::InitGLFW::~InitGLFW()
{
  glfwTerminate();
}

// -------------------------------------------------------------------------------------------------------------------------
ApplicationImpl::ApplicationImpl() :
  frame_(0), assets_(std::make_unique<asset::Database>()), frameSync_(MAX_FRAMES)
{
}

// -------------------------------------------------------------------------------------------------------------------------
ApplicationImpl::~ApplicationImpl()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::run(const std::string& dir)
{
  asset::Serializer::registerStandardAssets();
  asset::Serializer::deserialize(dir, assets_.get());
  auto props = assets_->get<asset::Application>("app");
  createWindow(props);
  checkValidationLayers();
  createInstance(props->title());
  setupLogging();
  createSurface();
  device_ = std::make_unique<Device>(*instance_, *surface_);
  allocator_ = std::make_unique<Allocator>(device_.get());
  createSwapchain({ props->width(), props->height() });
  createSync();    
  loop();
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::createSwapchain(VkExtent2D size)
{
  vkDeviceWaitIdle(**device_);
  model_.reset();
  swapchain_.reset();
  swapchain_ = std::make_unique<Swapchain>(device_.get(), allocator_.get(), *surface_, size);

  auto meshAsset = assets_->get<asset::Mesh>("mesh");
  mesh_ = std::make_unique<Mesh>(device_.get(), allocator_.get(), meshAsset);
  auto materialAsset = assets_->get<asset::Material>("material");
  material_ = std::make_unique<Material>(device_.get(), swapchain_.get(), materialAsset);
  auto textureAsset = assets_->get<asset::Texture>("texture");
  image_ = std::make_unique<Image>(device_.get(), allocator_.get(), textureAsset);
  imageView_ = std::make_unique<ImageView>(device_.get(), image_.get(), VK_IMAGE_ASPECT_COLOR_BIT);
  std::vector<ImageView*> textures = { imageView_.get() };
  model_ = std::make_unique<RenderObject>(device_.get(), swapchain_.get(), mesh_.get(), material_.get(), textures);
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::createSync()
{
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  auto deleteSemaphore = [this](auto semaphore) {
    vkDestroySemaphore(**device_, semaphore, nullptr);
    LOG(trace) << "Vulkan Semaphore destroyed";
  };
  auto deleteFence = [this](auto fence) {
    vkDestroyFence(**device_, fence, nullptr);
    LOG(trace) << "Vulkan Fence destroyed";
  };

  auto createSemaphore = [this, &semaphoreInfo](auto& semaphore) {
    if (vkCreateSemaphore(**device_, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
      THROW("Could not create Vulkan Semaphore");
    }
    LOG(trace) << "Vulkan Semaphore created";
  };

  VkFence fence;
  VkSemaphore semaphore;    
  for (size_t i = 0; i < frameSync_.size(); ++i) {
    createSemaphore(semaphore);
    frameSync_[i].render.set(semaphore, deleteSemaphore);
    createSemaphore(semaphore);
    frameSync_[i].present.set(semaphore, deleteSemaphore);
    if (vkCreateFence(**device_, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
      THROW("Could not create Vulkan Fence");
    }
    frameSync_[i].swap.set(fence, deleteFence);
    LOG(trace) << "Vulkan Fence created";
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::createWindow(asset::Application* props)
{
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  auto deleteWindow = [](auto window) {
    glfwDestroyWindow(window);
    LOG(trace) << "Window destroyed";
  };
  auto window = glfwCreateWindow(props->width(), props->height(), props->title().c_str(), nullptr, nullptr);
  if (!window) {
    THROW("Could not create Window");
  }
  window_.set(window, deleteWindow);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  LOG(trace) << "Window created";
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::loop()
{
  while (!glfwWindowShouldClose(*window_)) {
    glfwPollEvents();
    drawFrame();
  }
  vkDeviceWaitIdle(**device_);
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::createInstance(const std::string& appName)
{
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = appName.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "yaga";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
  if (validationLayers.empty()) {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  } else {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
    setDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  }

  auto extensions = getExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();
    
  auto deleteInstance = [](auto inst){
    vkDestroyInstance(inst, nullptr);
    LOG(trace) << "Vulkan instance destroyed";
  };
  VkInstance instance;
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    THROW("Could not create Vulkan Instance");
  }
  instance_.set(instance, deleteInstance);
  LOG(trace) << "Vulkan instance created";
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::createSurface()
{
  auto destroySurface = [this](auto surface) {
    vkDestroySurfaceKHR(*instance_, surface, nullptr);
    LOG(trace) << "Surface destroyed";
  };
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(*instance_, *window_, nullptr, &surface) != VK_SUCCESS) {
    THROW("Could not create Window Surface");
  }
  surface_.set(surface, destroySurface);
  LOG(trace) << "Surface created";
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::checkValidationLayers()
{
  if (validationLayers.empty()) return;

  // get available layers
  uint32_t count;
  vkEnumerateInstanceLayerProperties(&count, nullptr);
  std::vector<VkLayerProperties> availableLayers(count);
  vkEnumerateInstanceLayerProperties(&count, availableLayers.data());

  // sort available layers
  auto compareLayers = [](const auto& l1, const auto& l2) {
    return strncmp(l1.layerName, l2.layerName, VK_MAX_EXTENSION_NAME_SIZE) < 0;
  };
  std::sort(availableLayers.begin(), availableLayers.end(), compareLayers);

  // check that available layers contain requiered validationLayers
  for (const auto& layer : validationLayers) {
    VkLayerProperties props = {};
    strncpy(props.layerName, layer, VK_MAX_EXTENSION_NAME_SIZE);
    if (!std::binary_search(availableLayers.begin(), availableLayers.end(), props, compareLayers)) {
      THROW("Requested validation layer is not available");
    }
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::setupLogging()
{
  if (validationLayers.empty()) return;

  VkDebugUtilsMessengerCreateInfoEXT info = {};
  setDebugMessengerCreateInfo(info);
  auto CreateDebugUtilsMessenger = GET_EXT_PROC_ADDRESS(*instance_, vkCreateDebugUtilsMessengerEXT);

  auto DestroyDebugUtilsMessenger = GET_EXT_PROC_ADDRESS(*instance_, vkDestroyDebugUtilsMessengerEXT);
  auto destroyLogger = [this, DestroyDebugUtilsMessenger](VkDebugUtilsMessengerEXT messenger) {
    DestroyDebugUtilsMessenger(*instance_, messenger, nullptr);
    LOG(trace) << "Debug Messenger destroyed";
  };

  VkDebugUtilsMessengerEXT debugMessenger;
  if (CreateDebugUtilsMessenger(*instance_, &info, nullptr, &debugMessenger) != VK_SUCCESS) {
    THROW("Could not create Debug Messenger");
  }
  debugMessenger_.set(debugMessenger, destroyLogger);
  LOG(trace) << "Debug Messenger created";
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::drawFrame()
{
  const auto& sync = frameSync_[frame_];
  const auto swapchain = **swapchain_;

  vkWaitForFences(**device_, 1, &*sync.swap, VK_TRUE, UINT64_MAX);

  uint32_t index = 0;
  auto result = vkAcquireNextImageKHR(**device_, swapchain,
    UINT64_MAX, *sync.render, VK_NULL_HANDLE, &index);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    createSwapchain(getWindowSize());
    return;
  }
  if (result != VK_SUCCESS) {
    THROW("Failed to acquire swapchain image");
  }

  swapchain_->tmpUpdate(index);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &*sync.render;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &model_->commandBuffers()[index];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &*sync.present;

  vkResetFences(**device_, 1, &*sync.swap);
  if (vkQueueSubmit(device_->graphicsQueue(), 1, &submitInfo, *sync.swap) != VK_SUCCESS) {
    THROW("Could not draw frame");
  }

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &*sync.present;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain;
  presentInfo.pImageIndices = &index;

  vkQueuePresentKHR(device_->presentQueue(), &presentInfo);
  frame_ = (frame_ + 1) % MAX_FRAMES;

  if (resize_.resized) {
    resize_.resized = false;
    createSwapchain(resize_.size);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
VkExtent2D ApplicationImpl::getWindowSize() const
{
  int width = 0;
  int height = 0;
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(*window_, &width, &height);
    glfwWaitEvents();
  }
  return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

// -------------------------------------------------------------------------------------------------------------------------
void ApplicationImpl::resizeCallback(GLFWwindow* window, int width, int height)
{
  auto app = reinterpret_cast<ApplicationImpl*>(glfwGetWindowUserPointer(window));
  app->resize_.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
  app->resize_.resized = true;
}

} // !namespace yaga
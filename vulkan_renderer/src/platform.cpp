#include "precompiled.h"
#include "platform.h"
#include "device.h"
#include "swapchain.h"
#include "assets/material.h"
#include "assets/mesh.h"
#include "assets/texture.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#pragma warning(push, 0)
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#pragma warning(pop)

namespace yaga {
namespace vk {
namespace {

// const char* for compatibility with Valukan API
// -----------------------------------------------------------------------------------------------------------------------------
std::vector<const char*> getInstanceExtensions(VulkanExtensions& result)
{
  uint32_t glfwCount;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwCount);
  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwCount);
  if (!validationLayers.empty()) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  uint32_t availableCount;
  vkEnumerateInstanceExtensionProperties(nullptr, &availableCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(availableCount);
  if (availableCount > 0) {
    vkEnumerateInstanceExtensionProperties(nullptr, &availableCount, availableExtensions.data());
  }
  for (const auto& prop : availableExtensions) {
    if (strcmp(prop.extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0) {
      extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
      result.KHR_getPhysicalDeviceProperties2 = true;
    }
  }
  std::sort(extensions.begin(), extensions.end());
  return extensions;
}

// -----------------------------------------------------------------------------------------------------------------------------
VKAPI_ATTR VkBool32 VKAPI_CALL vulkanLog(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
  VkDebugUtilsMessageTypeFlagsEXT /*type*/, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* /*userData*/)
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

// -----------------------------------------------------------------------------------------------------------------------------
VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo()
{
  VkDebugUtilsMessengerCreateInfoEXT info{};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.messageSeverity =
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType =
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pfnUserCallback = vulkanLog;
  return info;
}

} // !namespace

Platform::InitGLFW Platform::initGLFW_;

// -----------------------------------------------------------------------------------------------------------------------------
PlatformPtr createPlatform(const assets::Application* asset)
{
  return std::make_unique<vk::Platform>(asset);
}

// -----------------------------------------------------------------------------------------------------------------------------
Platform::InitGLFW::InitGLFW()
{
  glfwInit();
}

// -----------------------------------------------------------------------------------------------------------------------------
Platform::InitGLFW::~InitGLFW()
{
  glfwTerminate();
}

// -----------------------------------------------------------------------------------------------------------------------------
Platform::Platform(const assets::Application* asset) :
  asset_(asset), minimised_(false), resized_(false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Platform::~Platform()
{
  eventDispatcher_->onResize(resizeConnection_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::run(Application* app)
{
  createWindow();
  checkValidationLayers();
  auto extensions = createInstance(asset_->title());
  setupLogging();
  createSurface();
  device_ = std::make_unique<Device>(*instance_, *surface_, extensions);
  createAllocator();
  VkExtent2D resolution{ asset_->width(), asset_->height() };
  swapchain_ = std::make_unique<Swapchain>(device_.get(), *allocator_, *surface_, resolution);
  renderingContext_ = std::make_unique<RenderingContext>(device_.get(), *allocator_, swapchain_.get(), asset_);
  renderer_ = std::make_unique<Renderer>(swapchain_.get(), renderingContext_.get());
  presenter_ = std::make_unique<Presenter>(device_.get(), swapchain_.get());
  setupGUI();
  app->init(renderingContext_.get(), input_.get());
  loop(app);
  app->shutdown();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::createWindow()
{
  namespace ph = std::placeholders;
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  auto deleteWindow = [](auto window) {
    glfwDestroyWindow(window);
    LOG(trace) << "Window destroyed";
  };
  auto window = glfwCreateWindow(asset_->width(), asset_->height(), asset_->title().c_str(), nullptr, nullptr);
  if (!window) {
    THROW("Could not create Window");
  }
  eventDispatcher_ = std::make_unique<EventDispatcher>(window);
  resizeConnection_ = eventDispatcher_->onResize(std::bind(&Platform::onResize, this, ph::_1, ph::_2));
  input_ = std::make_unique<Input>(window, eventDispatcher_.get());
  window_.set(window, deleteWindow);
  LOG(trace) << "Window created";
}

// -----------------------------------------------------------------------------------------------------------------------------
VulkanExtensions Platform::createInstance(const std::string& appName)
{
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = appName.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "yaga";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = GetVulkanApiVersion();

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  if (validationLayers.empty()) {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  } else {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
    auto debugCreateInfo = getDebugMessengerCreateInfo();
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  }

  VulkanExtensions extensions{};
  auto extensionNames = getInstanceExtensions(extensions);
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
  createInfo.ppEnabledExtensionNames = extensionNames.data();

  auto deleteInstance = [](auto inst) {
    vkDestroyInstance(inst, nullptr);
    LOG(trace) << "Vulkan instance destroyed";
  };
  VkInstance instance;
  VULKAN_GUARD(vkCreateInstance(&createInfo, nullptr, &instance), "Could not create Vulkan Instance");
  instance_.set(instance, deleteInstance);
  LOG(trace) << "Vulkan instance created";
  return extensions;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::createSurface()
{
  auto destroySurface = [this](auto surface) {
    vkDestroySurfaceKHR(*instance_, surface, nullptr);
    LOG(trace) << "Surface destroyed";
  };
  VkSurfaceKHR surface;
  VULKAN_GUARD(glfwCreateWindowSurface(*instance_, *window_, nullptr, &surface), "Could not create Window Surface");
  surface_.set(surface, destroySurface);
  LOG(trace) << "Surface created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::createAllocator()
{
  const auto& extensions = device_->extensions();
  VmaAllocatorCreateInfo info{};
  info.physicalDevice = device_->physical();
  info.device = **device_;
  info.instance = *instance_;
  info.vulkanApiVersion = GetVulkanApiVersion();
  if (extensions.KHR_dedicatedAllocation) {
    info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
  }
  if (extensions.KHR_bindMemory2) {
    info.flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
  }
  if (extensions.EXT_memoryBudget && extensions.KHR_getPhysicalDeviceProperties2) {
    info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
  }
  // TODO: check
  // if (extensions.AMD_deviceCoherentMemory) {
  //  info.flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
  //}
  auto destroyAllocator = [](VmaAllocator allocator) {
    vmaDestroyAllocator(allocator);
    LOG(trace) << "Video Memory Allocator destroyed";
  };
  VmaAllocator allocator;
  if (vmaCreateAllocator(&info, &allocator)) {
    THROW("Could not create Video Memory Allocator");
  }
  allocator_.set(allocator, destroyAllocator);
  LOG(trace) << "Video Memory Allocator created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::setupGUI()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io; // wtf is that?
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(*window_, true);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::resize(Application* app)
{
  vkDeviceWaitIdle(**device_);
  auto size = getWindowSize();
  if (size.width == 0 || size.height == 0) {
    minimised_ = true;
    return;
  }
  minimised_ = false;
  swapchain_.reset();
  swapchain_ = std::make_unique<Swapchain>(device_.get(), *allocator_, *surface_, size);
  renderingContext_->swapchain(swapchain_.get());
  renderer_->swapchain(swapchain_.get());
  presenter_->swapchain(swapchain_.get());
  app->resize();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::checkValidationLayers() const
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
    VkLayerProperties props{};
    strncpy(props.layerName, layer, VK_MAX_EXTENSION_NAME_SIZE);
    if (!std::binary_search(availableLayers.begin(), availableLayers.end(), props, compareLayers)) {
      THROW("Requested validation layer is not available");
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::setupLogging()
{
  if (validationLayers.empty()) return;

  auto CreateDebugUtilsMessenger = GET_EXT_PROC_ADDRESS(*instance_, vkCreateDebugUtilsMessengerEXT);
  auto DestroyDebugUtilsMessenger = GET_EXT_PROC_ADDRESS(*instance_, vkDestroyDebugUtilsMessengerEXT);
  auto destroyLogger = [this, DestroyDebugUtilsMessenger](VkDebugUtilsMessengerEXT messenger) {
    DestroyDebugUtilsMessenger(*instance_, messenger, nullptr);
    LOG(trace) << "Debug Messenger destroyed";
  };

  auto info = getDebugMessengerCreateInfo();
  VkDebugUtilsMessengerEXT debugMessenger;
  VULKAN_GUARD(CreateDebugUtilsMessenger(*instance_, &info, nullptr, &debugMessenger), "Could not create Debug Messenger");
  debugMessenger_.set(debugMessenger, destroyLogger);
  LOG(trace) << "Debug Messenger created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::loop(Application* app)
{
  startTime_ = std::chrono::high_resolution_clock::now();
  while (!glfwWindowShouldClose(*window_)) {
    glfwPollEvents();
    input_->updateState();
    gameLoop(app);
    drawFrame(app);
  }
  vkDeviceWaitIdle(**device_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::gameLoop(Application* app)
{
  auto currentTime = std::chrono::high_resolution_clock::now();
  auto delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime_).count();
  app->loop(delta);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::drawFrame(Application* app)
{
  if (resized_) {
    resized_ = false;
    resize(app);
  }
  if (minimised_) return;
  presenter_->waitPrevFrame();
  const auto mainCamera = renderingContext_->mainCamera();
  if (mainCamera == nullptr) return;
  uint32_t imageIndex = 0;
  if (!presenter_->acquireImage(&imageIndex)) {
    resize(app);
    return;
  }
  renderingContext_->update(imageIndex);
  renderer_->render(imageIndex);
  if (!presenter_->present(mainCamera->frame(imageIndex).command, imageIndex)) {
    resize(app);
    return;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
VkExtent2D Platform::getWindowSize() const
{
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(*window_, &width, &height);
  return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::onResize(int width, int height)
{
  if (minimised_ && width > 0 && height > 0) {
    resized_ = true;
  }
}

} // !namespace vk
} // !namespace yaga
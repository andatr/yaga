#include "precompiled.h"
#include "platform.h"
#include "device.h"
#include "renderer.h"
#include "render_pass_3d.h"
#include "render_stage_3d.h"
#include "render_stage_gui.h"
#include "swapchain.h"
#include "assets/material.h"
#include "assets/mesh.h"
#include "assets/texture.h"

#pragma warning(push, 0)
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#pragma warning(pop)

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
PlatformPtr createPlatform(const boost::property_tree::ptree& options)
{
  return std::make_unique<vk::Platform>(vk::Config(options));
}

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

// -----------------------------------------------------------------------------------------------------------------------------
void checkVkResult(VkResult err)
{
  if (err == 0)
    return;
  THROW("imgui error %1%", err);
}

} // !namespace

Platform::InitGLFW Platform::initGLFW_;

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
Platform::Platform(const Config& config) :
  config_(config),
  running_(false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Platform::~Platform()
{
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
void Platform::run(Application* app)
{
  if (running_) return;
  running_ = true;
  app_ = app;
  checkValidationLayers();
  auto extensions = createInstance(config_.title());
  setupLogging();
  window_ = std::make_unique<Window>(*instance_, config_);
  device_ = std::make_unique<Device>(*instance_, window_->surface(), extensions);
  createAllocator();
  VkExtent2D resolution{ config_.width(), config_.height() };
  swapchain_ = std::make_unique<Swapchain>(device_.get(), *allocator_, window_->surface(), resolution);
  device_->createDescriptorPool(swapchain_->imageCount(), config_);
  auto renderPass3D  = std::make_unique<RenderPass3D>(swapchain_.get());
  context_ = std::make_unique<Context>(swapchain_.get(), *allocator_, renderPass3D.get(), config_, app);
  std::vector<RenderStagePtr> stages(2);
  stages[0] = std::make_unique<RenderStage3D>(swapchain_.get(), *allocator_, std::move(renderPass3D));
  stages[1] = std::make_unique<RenderStageGui>(swapchain_.get(), window_.get(), config_);
  renderer_ = std::make_unique<Renderer>(swapchain_.get(), stages);
  app->init(context_.get(), window_->input());
  loop();
  app->shutdown();
  context_->clear();
  device_->waitIdle();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::loop()
{
  while (!glfwWindowShouldClose(**window_)) {
    window_->update();
    context_->update();
    if (!app_->loop()) break;
    if (window_->minimised()) continue;
    if (window_->resized() || !renderer_->render(context_.get())) {
      device_->waitIdle();
      window_->resized(false);
      swapchain_->resize(window_->size());
      app_->resize();
    }
  }
}

} // !namespace vk
} // !namespace yaga
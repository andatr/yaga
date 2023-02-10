#include "precompiled.h"
#include "vulkan_renderer/vulkan_utils.h"
#include "vulkan_renderer/swapchain.h"
#include <vk_mem_alloc.h>

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
VKAPI_ATTR VkBool32 VKAPI_CALL vulkanLog(
  VkDebugUtilsMessageSeverityFlagBitsEXT       severity,
  VkDebugUtilsMessageTypeFlagsEXT             /* type */, 
  const VkDebugUtilsMessengerCallbackDataEXT* callbackData, 
  void*                                       /* userData */
)
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
class InitGLFW
{
public:
  InitGLFW();
  ~InitGLFW();
  static InitGLFW& instance();
};

// -----------------------------------------------------------------------------------------------------------------------------
InitGLFW::InitGLFW()
{
  glfwInit();
}

// -----------------------------------------------------------------------------------------------------------------------------
InitGLFW::~InitGLFW()
{
  glfwTerminate();
}

// -----------------------------------------------------------------------------------------------------------------------------
InitGLFW& InitGLFW::instance()
{
  static InitGLFW instance;
  return instance;
}

} // !namespace

void checkValidationLayers();
AutoDestructor<VmaAllocator> createAllocator(VkInstance instance, Device* device);
AutoDestructor<VkInstance> createInstance(const std::string& appName, VulkanExtensions& extensions);
AutoDestructor<VkDebugUtilsMessengerEXT> setupLogging(VkInstance instance);

// -----------------------------------------------------------------------------------------------------------------------------
void initVulkan(
  ConfigPtr config,
  const SwapchainParams& swParams,
  AutoDestructor<VkInstance>& instance,
  WindowPtr& window,
  AutoDestructor<VkDebugUtilsMessengerEXT>& debugMessenger,
  DevicePtr& device,
  AutoDestructor<VmaAllocator>& allocator,
  SwapchainPtr& swapchain)
{
  InitGLFW::instance();
  checkValidationLayers();
  VulkanExtensions extensions{};
  instance = createInstance(config->window().title(), extensions);
  debugMessenger = setupLogging(*instance);
  window    = std::make_unique<Window>(*instance, config->window());
  device    = std::make_unique<Device>(*instance, window->surface(), extensions);
  allocator = createAllocator(*instance, device.get());
  swapchain = std::make_unique<Swapchain>(device.get(), *allocator, window->surface(), config, swParams);
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineShaderStageCreateInfo getShaderStageInfo(VkShaderModule module, VkShaderStageFlagBits type, const char* funcName)
{
  VkPipelineShaderStageCreateInfo info{};
  info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage  = type;
  info.module = module;
  info.pName  = funcName;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkSemaphore> createSemaphore(VkDevice device)
{
  auto deleteSemaphore = [device](auto semaphore) {
    vkDestroySemaphore(device, semaphore, nullptr);
    LOG(trace) << "Vulkan Semaphore destroyed";
  };
  VkSemaphore semaphore{};
  VkSemaphoreCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VULKAN_GUARD(vkCreateSemaphore(device, &info, nullptr, &semaphore), "Could not create Vulkan Semaphore");
  LOG(trace) << "Vulkan Semaphore created";
  return AutoDestructor<VkSemaphore>(semaphore, deleteSemaphore);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkFence> createFence(VkDevice device)
{
  auto deleteFence = [device](auto fence) {
    vkDestroyFence(device, fence, nullptr);
    LOG(trace) << "Vulkan Fence destroyed";
  };
  VkFence fence{};
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  VULKAN_GUARD(vkCreateFence(device, &fenceInfo, nullptr, &fence), "Could not create Vulkan Fence");
  LOG(trace) << "Vulkan Fence created";
  return AutoDestructor<VkFence>(fence, deleteFence);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkInstance> createInstance(const std::string& appName, VulkanExtensions& extensions)
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

  auto extensionNames = getInstanceExtensions(extensions);
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
  createInfo.ppEnabledExtensionNames = extensionNames.data();

  auto deleteInstance = [](auto inst) {
    vkDestroyInstance(inst, nullptr);
    LOG(trace) << "Vulkan instance destroyed";
  };
  VkInstance instance;
  VULKAN_GUARD(vkCreateInstance(&createInfo, nullptr, &instance), "Could not create Vulkan Instance");
  LOG(trace) << "Vulkan instance created";
  return AutoDestructor<VkInstance>(instance, deleteInstance);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VmaAllocator> createAllocator(VkInstance instance, Device* device)
{
  const auto& extensions = device->extensions();
  VmaAllocatorCreateInfo info{};
  info.physicalDevice = device->physical();
  info.device = **device;
  info.instance = instance;
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
  LOG(trace) << "Video Memory Allocator created";
  return AutoDestructor<VmaAllocator>(allocator, destroyAllocator);
}

// -----------------------------------------------------------------------------------------------------------------------------
void checkValidationLayers()
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
AutoDestructor<VkDebugUtilsMessengerEXT> setupLogging(VkInstance instance)
{
  if (validationLayers.empty()) {
    return AutoDestructor<VkDebugUtilsMessengerEXT>();
  }
  auto createDebugUtilsMessenger  = GET_EXT_PROC_ADDRESS(instance, vkCreateDebugUtilsMessengerEXT);
  auto destroyDebugUtilsMessenger = GET_EXT_PROC_ADDRESS(instance, vkDestroyDebugUtilsMessengerEXT);
  auto destroyLogger = [instance, destroyDebugUtilsMessenger](VkDebugUtilsMessengerEXT messenger) {
    destroyDebugUtilsMessenger(instance, messenger, nullptr);
    LOG(trace) << "Debug Messenger destroyed";
  };
  auto info = getDebugMessengerCreateInfo();
  VkDebugUtilsMessengerEXT debugMessenger;
  VULKAN_GUARD(createDebugUtilsMessenger(instance, &info, nullptr, &debugMessenger), "Could not create Debug Messenger");
  LOG(trace) << "Debug Messenger created";
  return AutoDestructor<VkDebugUtilsMessengerEXT>(debugMessenger, destroyLogger);
}

// -----------------------------------------------------------------------------------------------------------------------------
VkDescriptorSet createDescriptor(VkDevice device, const VkDescriptorSetAllocateInfo& info)
{
  if (info.descriptorSetCount > 1) THROW("vk::createDescriptor called with count > 1");
  VkDescriptorSet descriptor{};
  VULKAN_GUARD(vkAllocateDescriptorSets(device, &info, &descriptor), "Could not create DescriptorSet");
  LOG(trace) << "DescriptorSet created";
  return descriptor;
}

// -----------------------------------------------------------------------------------------------------------------------------
std::vector<VkDescriptorSet> createDescriptors(VkDevice device, const VkDescriptorSetAllocateInfo& info)
{
  std::vector<VkDescriptorSet> descriptors(info.descriptorSetCount);
  VULKAN_GUARD(vkAllocateDescriptorSets(device, &info, descriptors.data()), "Could not create DescriptorSets");
  LOG(trace) << info.descriptorSetCount << " DescriptorSets created";
  return descriptors;
}

// -----------------------------------------------------------------------------------------------------------------------------
void updateDescriptor(VkDevice device, const VkWriteDescriptorSet* writers, size_t count)
{
  const auto count32 = static_cast<uint32_t>(count);
  vkUpdateDescriptorSets(device, count32, writers, 0, nullptr);
  LOG(trace) << count << " DescriptorSet(s) updated";
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkSampler> createSampler(VkDevice device, const VkSamplerCreateInfo& info)
{
  auto destroySampler = [device](auto sampler) {
    vkDestroySampler(device, sampler, nullptr);
    LOG(trace) << "Image Sampler destroyed " << sampler;
  };
  VkSampler sampler{};
  VULKAN_GUARD(vkCreateSampler(device, &info, nullptr, &sampler), "Could not create Image Sampler");
  LOG(trace) << "Image Sampler created " << sampler;
  return AutoDestructor<VkSampler>(sampler, destroySampler);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkDescriptorSetLayout> createLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo& info)
{
  auto destroyLayout = [device](auto layout) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    LOG(trace) << "Descriptor Set destroyed " << layout;
  };
  VkDescriptorSetLayout layout{};
  VULKAN_GUARD(vkCreateDescriptorSetLayout(device, &info, nullptr, &layout), "Could not create Descriptor Set Layout");
  LOG(trace) << "Descriptor Set Layout created " << layout;
  return AutoDestructor<VkDescriptorSetLayout>(layout, destroyLayout);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkPipelineLayout> createLayout(VkDevice device, const VkPipelineLayoutCreateInfo& info)
{
  auto destroyLayout = [device](auto layout) {
    vkDestroyPipelineLayout(device, layout, nullptr);
    LOG(trace) << "Pipeline Layout destroyed " << layout;
  };
  VkPipelineLayout layout{};
  VULKAN_GUARD(vkCreatePipelineLayout(device, &info, nullptr, &layout), "Could not create Pipeline Layout");
  LOG(trace) << "Pipeline Layout created " << layout ;
  return AutoDestructor<VkPipelineLayout>(layout, destroyLayout);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkRenderPass> createRenderPass(VkDevice device, const VkRenderPassCreateInfo& info)
{
  auto destroyRenderPass = [device](auto renderPass) {
    vkDestroyRenderPass(device, renderPass, nullptr);
    LOG(trace) << "Render Pass destroyed " << renderPass;
  };
  VkRenderPass renderPass{};
  VULKAN_GUARD(vkCreateRenderPass(device, &info, nullptr, &renderPass), "Could not create Render Pass");
  LOG(trace) << "Render Pass created " << renderPass;
  return AutoDestructor<VkRenderPass>(renderPass, destroyRenderPass);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkShaderModule> createShader(VkDevice device, const VkShaderModuleCreateInfo& info)
{
  auto destroyShader = [device](auto shader) {
    vkDestroyShaderModule(device, shader, nullptr);
    LOG(trace) << "Shader destroyed " << shader;
  };
  VkShaderModule shader{};
  VULKAN_GUARD(vkCreateShaderModule(device, &info, nullptr, &shader), "Could not create Shader");
  LOG(trace) << "Shader created " << shader;
  return AutoDestructor<VkShaderModule>(shader, destroyShader);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkPipeline> createPipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& info)
{
  auto destroyPipeline = [device](VkPipeline pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
    LOG(trace) << "Pipeline destroyed " << pipeline;
  };
  VkPipeline pipeline{};
  VULKAN_GUARD(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline), "Could not create Pipeline");
  LOG(trace) << "Pipeline created " << pipeline;
  return AutoDestructor<VkPipeline>(pipeline, destroyPipeline);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkPipeline> createPipeline(VkDevice device, const VkComputePipelineCreateInfo& info)
{
  auto destroyPipeline = [device](VkPipeline pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
    LOG(trace) << "Compute Pipeline destroyed " << pipeline;
  };
  VkPipeline pipeline{};
  VULKAN_GUARD(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline), "Could not create Compute Pipeline");
  LOG(trace) << "Compute Pipeline created " << pipeline;
  return AutoDestructor<VkPipeline>(pipeline, destroyPipeline);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkCommandPool> createCommandPool(VkDevice device, const VkCommandPoolCreateInfo& info)
{
  VkCommandPool commandPool{};
  auto destroyCommandPool = [device](auto commandPool) {
    vkDestroyCommandPool(device, commandPool, nullptr);
    LOG(trace) << "Command Pool destroyed (" << commandPool;
  };
  VULKAN_GUARD(vkCreateCommandPool(device, &info, nullptr, &commandPool), "Could not create Command Pool");
  LOG(trace) << "Command Pool created " << commandPool;
  return AutoDestructor<VkCommandPool>(commandPool, destroyCommandPool);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkDescriptorPool> createDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo& info)
{
  VkDescriptorPool pool{};
  auto destroyPool = [device](auto pool) {
    vkDestroyDescriptorPool(device, pool, nullptr);
    LOG(trace) << "Descriptor Pool destroyed " << pool;
  };
  VULKAN_GUARD(vkCreateDescriptorPool(device, &info, nullptr, &pool), "Could not create Descriptor Pool");
  LOG(trace) << "Descriptor Pool created " << pool;
  return AutoDestructor<VkDescriptorPool>(pool, destroyPool);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkCommandBuffer> createCommand(VkDevice device, VkCommandPool pool, const VkCommandBufferAllocateInfo& info)
{
  VkCommandBuffer command{};
  auto destroyCommand = [device, pool](auto command) {
    vkFreeCommandBuffers(device, pool, 1, &command);
    LOG(trace) << "Command Buffer destroyed " << command;
  };
  VULKAN_GUARD(vkAllocateCommandBuffers(device, &info, &command), "Could not create Command Buffer");
  LOG(trace) << "Command Buffer created " << command;
  return AutoDestructor<VkCommandBuffer>(command, destroyCommand);
}

// -----------------------------------------------------------------------------------------------------------------------------
void insertDescriptorPoolSize(std::vector<VkDescriptorPoolSize>& sizes, VkDescriptorType type, uint32_t count)
{
  bool found = false;
  for (int i = 0; i < sizes.size(); ++i) {
    if (sizes[i].type == type) {
      found = true;
      sizes[i].descriptorCount += count;
    }
  }
  if (!found) {
    sizes.push_back({ type, count });
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkImage> createImage(VmaAllocator allocator, const VkImageCreateInfo& info)
{
  VmaAllocation allocation{};
  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkImage image{};
  VULKAN_GUARD(vmaCreateImage(allocator, &info, &allocInfo, &image, &allocation, nullptr), "Could not create Vulkan Image");
  LOG(trace) << "Vulkan Image created " << image;

  auto destroyImage = [allocator, allocation](auto image) {
    vmaDestroyImage(allocator, image, allocation);
    LOG(trace) << "Vulkan Image destroyed " << image;
  };

  return AutoDestructor<VkImage>(image, destroyImage);
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkImageView> createImageView(VkDevice device, const VkImageViewCreateInfo& info)
{
  VkImageView view{};
  auto destroyView = [device](auto view) {
    vkDestroyImageView(device, view, nullptr);
    LOG(trace) << "Vulkan Image View destroyed " << view;
  };
  VULKAN_GUARD(vkCreateImageView(device, &info, nullptr, &view), "Could not create Vulkan Image View");
  LOG(trace) << "Vulkan Image View created " << view;
  return AutoDestructor<VkImageView>(view, destroyView);
}

// -----------------------------------------------------------------------------------------------------------------------------
void copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandBuffer command)
{
  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(command, source, destination, 1, &copyRegion);
}

} // !namespace vk
} // !namespace yaga
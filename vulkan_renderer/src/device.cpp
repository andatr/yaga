#include "precompiled.h"
#include "device.h"
#include "swapchain.h"

namespace yaga {
namespace vk {
namespace {

const std::set<std::string> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

struct DeviceFeatures
{
  std::vector<uint32_t> graphics;
  std::vector<uint32_t> compute;
  std::vector<uint32_t> transfer;
  std::vector<uint32_t> sparceMemory;
  std::vector<uint32_t> protectedMemory;
  std::vector<uint32_t> surface;
};

// -----------------------------------------------------------------------------------------------------------------------------
std::vector<std::string> getDeviceExtensions(VkPhysicalDevice device)
{
  uint32_t count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> extensions(count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());
  std::vector<std::string> result;
  std::transform(
    extensions.begin(), extensions.end(), std::back_inserter(result), [](const auto& prop) { return prop.extensionName; });
  std::sort(result.begin(), result.end());
  return result;
}

// -----------------------------------------------------------------------------------------------------------------------------
DeviceFeatures getDeviceFeatures(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  uint32_t count;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> properties(count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());

  DeviceFeatures features{};
  for (uint32_t i = 0; i < (uint32_t)properties.size(); ++i) {
    const auto& props = properties[i];
    if (props.queueCount == 0) continue;
    if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      features.graphics.push_back(i);
    }
    if (props.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      features.compute.push_back(i);
    }
    if (props.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      features.transfer.push_back(i);
    }
    if (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
      features.sparceMemory.push_back(i);
    }
    if (props.queueFlags & VK_QUEUE_PROTECTED_BIT) {
      features.protectedMemory.push_back(i);
    }
    VkBool32 surfaceSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surfaceSupport);
    if (surfaceSupport) {
      features.surface.push_back(i);
    }
  }
  return features;
}

// -----------------------------------------------------------------------------------------------------------------------------
bool checkDeviceFeatures(VkPhysicalDevice device, const DeviceFeatures& features, const std::vector<std::string>& extensions)
{
  VkPhysicalDeviceFeatures physicalFeatures;
  vkGetPhysicalDeviceFeatures(device, &physicalFeatures);
  if (features.graphics.empty() || features.surface.empty() || features.compute.empty() || features.transfer.empty() ||
      !physicalFeatures.samplerAnisotropy) {
    return false;
  }
  return std::includes(extensions.begin(), extensions.end(), requiredExtensions.begin(), requiredExtensions.end());
}

// -----------------------------------------------------------------------------------------------------------------------------
std::vector<const char*> filterExtensions(const std::vector<std::string>& extensions, VulkanExtensions& result)
{
  // TODO: add support for following extensions:
  // VK_EXT_buffer_device_address
  // VK_KHR_buffer_device_address
  // Sparse Binding
  // Buffer Device Address
  std::vector<const char*> filtered;
  for (const auto& extension : extensions) {
    if (extension == VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME) {
      result.KHR_getMemoryRequirements2 = true;
      filtered.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
    } else if (extension == VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME) {
      result.KHR_dedicatedAllocation = true;
      filtered.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
    } else if (extension == VK_KHR_BIND_MEMORY_2_EXTENSION_NAME) {
      result.KHR_bindMemory2 = true;
      filtered.push_back(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
    } else if (result.KHR_getPhysicalDeviceProperties2 && extension == VK_EXT_MEMORY_BUDGET_EXTENSION_NAME) {
      result.EXT_memoryBudget = true;
      filtered.push_back(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
    } else if (extension == VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME) {
      result.AMD_deviceCoherentMemory = true;
      filtered.push_back(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME);
    }
  }
  return filtered;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Device::Device(VkInstance instance, VkSurfaceKHR surface, const VulkanExtensions& extensions) :
  physicalDevice_(VK_NULL_HANDLE), properties_{}, memoryProperties_{}, queueFamilies_{}, queues_{}, extensions_(extensions)
{
  // find all available devices and their properties
  uint32_t count;
  vkEnumeratePhysicalDevices(instance, &count, nullptr);
  std::vector<VkPhysicalDevice> devices(count);
  vkEnumeratePhysicalDevices(instance, &count, devices.data());
  DeviceFeatures features{};
  std::vector<std::string> extensionNames;
  auto it = std::find_if(devices.begin(), devices.end(), [&surface, &features, &extensionNames](const auto& device) {
    features = getDeviceFeatures(device, surface);
    extensionNames = getDeviceExtensions(device);
    return checkDeviceFeatures(device, features, extensionNames);
  });
  if (it == devices.end()) {
    THROW("Could not find GPU that supports Vulkan");
  }
  physicalDevice_ = *it;
  auto filteredExtensions = filterExtensions(extensionNames, extensions_);
  // TODO: we probably want to pick unique index for each queue
  queueFamilies_.graphics = features.graphics[0];
  queueFamilies_.surface = features.surface[0];
  queueFamilies_.compute = features.compute[0];
  queueFamilies_.transfer = features.transfer[0];
  createDevice(filteredExtensions);
  vkGetPhysicalDeviceProperties(physicalDevice_, &properties_);
  vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memoryProperties_);
  createCommandPool();
  createImmediateCommand();
}

// -----------------------------------------------------------------------------------------------------------------------------
Device::~Device()
{}

// -----------------------------------------------------------------------------------------------------------------------------
void Device::waitIdle() const
{
  vkDeviceWaitIdle(*logicalDevice_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Device::createDevice(std::vector<const char*>& extensions)
{
  std::vector<VkDeviceQueueCreateInfo> queueInfos;
  float priority = 1.0f;
  std::set<uint32_t> families = { queueFamilies_.graphics, queueFamilies_.compute, queueFamilies_.transfer };
  for (uint32_t family : families) {
    VkDeviceQueueCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    info.queueFamilyIndex = family;
    info.queueCount = 1;
    info.pQueuePriorities = &priority;
    queueInfos.push_back(info);
  }
  VkPhysicalDeviceFeatures features{};
  features.fillModeNonSolid = VK_TRUE;
  features.samplerAnisotropy = VK_TRUE;
  VkDeviceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.pQueueCreateInfos = queueInfos.data();
  info.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
  info.pEnabledFeatures = &features;
  for (const auto& ext : requiredExtensions) {
    extensions.push_back(ext.c_str()); // dirty but safe
  }
  info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  info.ppEnabledExtensionNames = extensions.data();
  if (validationLayers.empty()) {
    info.enabledLayerCount = 0;
  } else {
    info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    info.ppEnabledLayerNames = validationLayers.data();
  }

  auto destroyDevice = [](auto device) {
    vkDestroyDevice(device, nullptr);
    LOG(trace) << "Logical Device destroyed";
  };
  VkDevice device;
  VULKAN_GUARD(vkCreateDevice(physicalDevice_, &info, nullptr, &device), "Could not create Vulkan device");
  logicalDevice_.set(device, destroyDevice);
  LOG(trace) << "Logical Device created";

  vkGetDeviceQueue(*logicalDevice_, queueFamilies_.graphics, 0, queues_ + 0);
  vkGetDeviceQueue(*logicalDevice_, queueFamilies_.surface,  0, queues_ + 1);
  vkGetDeviceQueue(*logicalDevice_, queueFamilies_.transfer, 0, queues_ + 2);
  vkGetDeviceQueue(*logicalDevice_, queueFamilies_.compute,  0, queues_ + 3);
}

// -----------------------------------------------------------------------------------------------------------------------------
uint32_t Device::getMemoryType(uint32_t filter, VkMemoryPropertyFlags props) const
{
  for (uint32_t i = 0; i < memoryProperties_.memoryTypeCount; i++) {
    if (filter & (1 << i) && (memoryProperties_.memoryTypes[i].propertyFlags & props) == props) {
      return i;
    }
  }
  THROW("Could not find suitable memory type");
}

// -----------------------------------------------------------------------------------------------------------------------------
void Device::createCommandPool()
{
  VkCommandPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = queueFamilies_.graphics;

  VkCommandPool commandPool;
  auto destroyCommandPool = [this](auto commandPool) {
    vkDestroyCommandPool(*logicalDevice_, commandPool, nullptr);
    LOG(trace) << "Command Pool destroyed";
  };
  VULKAN_GUARD(vkCreateCommandPool(*logicalDevice_, &info, nullptr, &commandPool), "Could not create Command Pool");
  commandPool_.set(commandPool, destroyCommandPool);
  LOG(trace) << "Command Pool created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Device::createDescriptorPool(uint32_t frames, const assets::Application* limits)
{
  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = frames;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = limits->maxTextureCount() * frames;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = 50; // TODO: FIX !

  VkDescriptorPool pool;
  auto destroyPool = [device = *logicalDevice_](auto pool) {
    vkDestroyDescriptorPool(device, pool, nullptr);
    LOG(trace) << "Descriptor Pool destroyed";
  };

  VULKAN_GUARD(vkCreateDescriptorPool(*logicalDevice_, &poolInfo, nullptr, &pool), "Could not create descriptor pool");
  descriptorPool_.set(pool, destroyPool);
  LOG(trace) << "Descriptor Pool created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Device::createImmediateCommand()
{
  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  info.commandPool = *commandPool_;
  info.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  auto destroyCommandBuffer = [this](auto commandBuffer) {
    vkFreeCommandBuffers(*logicalDevice_, *commandPool_, 1, &commandBuffer);
    LOG(trace) << "Immediate Command Buffer destroyed";
  };
  VULKAN_GUARD(vkAllocateCommandBuffers(*logicalDevice_, &info, &commandBuffer), "Could not create Command Buffer");
  immediateCommand_.set(commandBuffer, destroyCommandBuffer);
  LOG(trace) << "Immediate Command Buffer created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Device::submitCommand(const CommandHandler& handler) const
{
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VULKAN_GUARD(vkBeginCommandBuffer(*immediateCommand_, &beginInfo), "Could not begin Vulkan Command");
  handler(*immediateCommand_);
  VULKAN_GUARD(vkEndCommandBuffer(*immediateCommand_), "Could not finish Vulkan Command");

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &*immediateCommand_;

  VULKAN_GUARD(vkQueueSubmit(graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE), "Could not submit Vulkan Command");
  VULKAN_GUARD(vkQueueWaitIdle(graphicsQueue()), "Could not submit Vulkan Command");
}

} // !namespace vk
} // !namespace yaga
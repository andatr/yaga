#include "precompiled.h"
#include "device.h"
#include "extensions.h"
#include "swapchain.h"

namespace yaga
{
namespace
{

// I'm trying to avoid global variables with non-trivial destructor
// but need number of elements later
const std::vector<char*> deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct DeviceFeatures
{
  VkPhysicalDevice device;
  std::vector<uint32_t> graphics;
  std::vector<uint32_t> compute;
  std::vector<uint32_t> transfer;
  std::vector<uint32_t> sparceMemory;
  std::vector<uint32_t> protectedMemory;
  std::vector<uint32_t> surface;
};

// -------------------------------------------------------------------------------------------------------------------------
std::vector<std::string> getDeviceExtensions(VkPhysicalDevice device)
{
  uint32_t count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> extensions(count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());
  std::vector<std::string> result;
  std::transform(extensions.begin(), extensions.end(), std::back_inserter(result),
    [] (const auto& prop) { return prop.extensionName; }
  );
  std::sort(result.begin(), result.end());
  return result;
}

// -------------------------------------------------------------------------------------------------------------------------
DeviceFeatures getDeviceFeatures(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  uint32_t count;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> properties(count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());

  DeviceFeatures families = { device };
  for (uint32_t i = 0; i < (uint32_t)properties.size(); ++i) {
    const auto& props = properties[i];
    if (props.queueCount == 0) continue;
    if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      families.graphics.push_back(i);
    }
    if (props.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      families.compute.push_back(i);
    }
    if (props.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      families.transfer.push_back(i);
    }
    if (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
      families.sparceMemory.push_back(i);
    }
    if (props.queueFlags & VK_QUEUE_PROTECTED_BIT) {
      families.protectedMemory.push_back(i);
    }
    VkBool32 surfaceSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surfaceSupport);
    if (surfaceSupport) {
      families.surface.push_back(i);
    }
  }
  return families;
}

// -------------------------------------------------------------------------------------------------------------------------
bool checkDeviceFeatures(const DeviceFeatures& features, const std::set<std::string>& requiredExtensions)
{
  VkPhysicalDeviceFeatures physicalFeatures;
  vkGetPhysicalDeviceFeatures(features.device, &physicalFeatures);

  if (features.graphics.empty() ||
      features.surface.empty()  ||
      features.compute.empty()  ||
      features.transfer.empty() ||
      !physicalFeatures.samplerAnisotropy)
  {
    return false;
  }
  auto actualExtensions = getDeviceExtensions(features.device);
  return std::includes(actualExtensions.begin(), actualExtensions.end(), requiredExtensions.begin(), requiredExtensions.end());
}

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
Device::Device(VkInstance instance, VkSurfaceKHR surface) :
  physicalDevice_(VK_NULL_HANDLE), memoryProperties_{}, queueFamilies_{}, queues_{}
{
  // find all available devices and their properties
  uint32_t count;
  vkEnumeratePhysicalDevices(instance, &count, nullptr);
  std::vector<VkPhysicalDevice> devices(count);
  vkEnumeratePhysicalDevices(instance, &count, devices.data());
  std::vector<DeviceFeatures> devicesWithFeatures;
  std::transform(devices.begin(), devices.end(), std::back_inserter(devicesWithFeatures),
    [surface](const auto& device) { return getDeviceFeatures(device, surface); }
  );

  // pick one device that supports our requirements
  std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end()); // need it sorted and unique ...
  auto it = std::find_if(devicesWithFeatures.begin(), devicesWithFeatures.end(),
    [requiredExtensions](const auto& device) { return checkDeviceFeatures(device, requiredExtensions); }
  );
  if (it == devicesWithFeatures.end()) {
    THROW("Could not find GPU that supports Vulkan");
  }
  physicalDevice_ = it->device;
  // ToDo: we probably want to pick unique index for each queue
  queueFamilies_.graphics = it->graphics[0];
  queueFamilies_.surface  = it->surface[0];
  queueFamilies_.compute  = it->compute[0];
  queueFamilies_.transfer = it->transfer[0];
  createDevice();
  vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memoryProperties_);
  createCommandPool();
}

// -------------------------------------------------------------------------------------------------------------------------
Device::~Device()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Device::createDevice()
{
  std::vector<VkDeviceQueueCreateInfo> queueInfos;
  
  float priority = 1.0f;
  std::set<uint32_t> families = { queueFamilies_.graphics, queueFamilies_.compute, queueFamilies_.transfer };
  for (uint32_t family : families) {
    VkDeviceQueueCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    info.queueFamilyIndex = family;
    info.queueCount = 1;
    info.pQueuePriorities = &priority;
    queueInfos.push_back(info);
  }

  VkPhysicalDeviceFeatures features = {};
  features.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.pQueueCreateInfos = queueInfos.data();
  info.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
  info.pEnabledFeatures = &features;
  info.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  info.ppEnabledExtensionNames = deviceExtensions.data();
  if (validationLayers.empty()) {
    info.enabledLayerCount = 0;
  }
  else {
    info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    info.ppEnabledLayerNames = validationLayers.data();
  }

  auto destroyDevice = [](auto device) {
    vkDestroyDevice(device, nullptr);
    LOG(trace) << "Logical Device destroyed";
  };
  VkDevice device;
  if (vkCreateDevice(physicalDevice_, &info, nullptr, &device) != VK_SUCCESS) {
    THROW("Could not create Vulkan device");
  }
  logicalDevice_.set(device, destroyDevice);
  LOG(trace) << "Logical Device created";

  vkGetDeviceQueue(*logicalDevice_, queueFamilies_.graphics, 0, queues_ + 0);
  vkGetDeviceQueue(*logicalDevice_, queueFamilies_.surface,  0, queues_ + 1);
  vkGetDeviceQueue(*logicalDevice_, queueFamilies_.transfer, 0, queues_ + 2);
  vkGetDeviceQueue(*logicalDevice_, queueFamilies_.compute,  0, queues_ + 3);
}

// -------------------------------------------------------------------------------------------------------------------------
uint32_t Device::getMemoryType(uint32_t filter, VkMemoryPropertyFlags props) const
{
  for (uint32_t i = 0; i < memoryProperties_.memoryTypeCount; i++) {
    if (filter & (1 << i) && (memoryProperties_.memoryTypes[i].propertyFlags & props) == props) {
      return i;
    }
  }
  THROW("Could not find suitable memory type");
}

// -------------------------------------------------------------------------------------------------------------------------
void Device::createCommandPool()
{
  VkCommandPoolCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.queueFamilyIndex = queueFamilies_.graphics;

  VkCommandPool commandPool;
  auto destroyCommandPool = [this](auto commandPool) {
    vkDestroyCommandPool(*logicalDevice_, commandPool, nullptr);
    LOG(trace) << "Command Pool destroyed";
  };
  if (vkCreateCommandPool(*logicalDevice_, &info, nullptr, &commandPool) != VK_SUCCESS) {
    THROW("Could not create Command Pool");
  }
  commandPool_.set(commandPool, destroyCommandPool);
}

// -------------------------------------------------------------------------------------------------------------------------
void Device::submitCommand(CommandHandler handler) const
{
  VkCommandBufferAllocateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  info.commandPool = *commandPool_;
  info.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(*logicalDevice_, &info, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);
  handler(commandBuffer);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  
  vkQueueSubmit(graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue());
  vkFreeCommandBuffers(*logicalDevice_, *commandPool_, 1, &commandBuffer);
}

} // !namespace yaga
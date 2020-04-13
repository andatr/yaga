#include "precompiled.h"
#include "allocator.h"

namespace yaga
{
namespace vk
{

// -------------------------------------------------------------------------------------------------------------------------
Allocator::Allocator(Device* device) :
  device_(device), vkDevice(**device)
{
}

// -------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkDeviceMemory> Allocator::allocate(VkBuffer buffer, VkMemoryPropertyFlags properties) const
{
  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(vkDevice, buffer, &requirements);
  return allocate(requirements, properties);
}

// -------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkDeviceMemory> Allocator::allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties) const
{
  VkMemoryAllocateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info.allocationSize = requirements.size;
  info.memoryTypeIndex = device_->getMemoryType(requirements.memoryTypeBits, properties);

  auto freeMemory = [device = vkDevice](auto memory) {
    vkFreeMemory(device, memory, nullptr);
    LOG(trace) << "Graphics device memory released";
  };
  VkDeviceMemory memory;
  if (vkAllocateMemory(vkDevice, &info, nullptr, &memory) != VK_SUCCESS) {
    THROW("Could not allocate graphics device memory");
  }
  AutoDestructor<VkDeviceMemory> smartMemory;
  smartMemory.set(memory, freeMemory);
  return smartMemory;
}

} // !namespace vk
} // !namespace yaga
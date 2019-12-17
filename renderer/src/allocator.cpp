#include "precompiled.h"
#include "allocator.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Allocator::Allocator(Device* device) : device_(device)
{
}

// -------------------------------------------------------------------------------------------------------------------------
AutoDestroyer<VkDeviceMemory> Allocator::Allocate(VkBuffer buffer, VkMemoryPropertyFlags properties) const
{
  const auto device = device_->Logical();
  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(device, buffer, &requirements);
  return Allocate(requirements, properties);
}

// -------------------------------------------------------------------------------------------------------------------------
AutoDestroyer<VkDeviceMemory> Allocator::AllocateImage(VkImage image, VkMemoryPropertyFlags properties) const
{
  const auto device = device_->Logical();
  VkMemoryRequirements requirements;
  vkGetImageMemoryRequirements(device, image, &requirements);
  return Allocate(requirements, properties);
}

// -------------------------------------------------------------------------------------------------------------------------
AutoDestroyer<VkDeviceMemory> Allocator::Allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties) const
{
  const auto device = device_->Logical();
  VkMemoryAllocateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info.allocationSize = requirements.size;
  info.memoryTypeIndex = device_->GetMemoryType(requirements.memoryTypeBits, properties);

  auto freeMemory = [device](auto memory) {
    vkFreeMemory(device, memory, nullptr);
    LOG(trace) << "Graphics device memory released";
  };
  VkDeviceMemory memory;
  if (vkAllocateMemory(device, &info, nullptr, &memory) != VK_SUCCESS) {
    THROW("Could not allocate graphics device memory");
  }
  AutoDestroyer<VkDeviceMemory> smartMemory;
  smartMemory.Assign(memory, freeMemory);
  return smartMemory;
}

} // !namespace yaga
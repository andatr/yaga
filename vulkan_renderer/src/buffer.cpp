#include "precompiled.h"
#include "buffer.h"

namespace yaga
{
namespace vk
{

// -------------------------------------------------------------------------------------------------------------------------
Buffer::Buffer(VmaAllocator allocator, const VkBufferCreateInfo& info, const VmaAllocationCreateInfo& allocInfo) :
  allocator_(allocator), buffer_(VK_NULL_HANDLE), allocation_{}, memory_{}
{
  VULKAN_GUARD(vmaCreateBuffer(allocator_, &info, &allocInfo, &buffer_, &allocation_, &memory_),
    "Could not create Vulkan Buffer Object");
}

// -------------------------------------------------------------------------------------------------------------------------
Buffer::~Buffer()
{
  vmaDestroyBuffer(allocator_, buffer_, allocation_);
  LOG(trace) << "Vulkan Buffer Object destroyed";
}

} // !namespace vk
} // !namespace yaga
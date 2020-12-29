#include "precompiled.h"
#include "buffer.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Buffer::Buffer(VmaAllocator allocator, const VkBufferCreateInfo& info, const VmaAllocationCreateInfo& allocInfo) :
  allocator_(allocator),
  buffer_(VK_NULL_HANDLE)
{
  VULKAN_GUARD(vmaCreateBuffer(allocator_, &info, &allocInfo, &buffer_, &allocation_, &memory_),
    "Could not create Buffer Object");
  LOG(trace) << "Buffer Object created";
}

// -----------------------------------------------------------------------------------------------------------------------------
Buffer::~Buffer()
{
  vmaDestroyBuffer(allocator_, buffer_, allocation_);
  LOG(trace) << "Buffer Object destroyed";
}

} // !namespace vk
} // !namespace yaga
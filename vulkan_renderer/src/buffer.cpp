#include "precompiled.h"
#include "vulkan_renderer/buffer.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Buffer::Buffer(VmaAllocator allocator, const VkBufferCreateInfo& info, const VmaAllocationCreateInfo& allocInfo) :
  allocator_(allocator),
  buffer_(VK_NULL_HANDLE)
{
  update(info, allocInfo);
}

// -----------------------------------------------------------------------------------------------------------------------------
Buffer::~Buffer()
{
  clear();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Buffer::update(const VkBufferCreateInfo& info, const VmaAllocationCreateInfo& allocInfo)
{
  clear();
  VULKAN_GUARD(vmaCreateBuffer(allocator_, &info, &allocInfo, &buffer_, &allocation_, &memory_),
    "Could not create Buffer Object");
  LOG(trace) << "Buffer Object created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Buffer::clear()
{
  if (!buffer_) return;
  vmaDestroyBuffer(allocator_, buffer_, allocation_);
  LOG(trace) << "Buffer Object destroyed";
}

} // !namespace vk
} // !namespace yaga
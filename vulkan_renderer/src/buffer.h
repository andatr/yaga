#ifndef YAGA_VULKAN_RENDERER_SRC_BUFFER
#define YAGA_VULKAN_RENDERER_SRC_BUFFER

#include <memory>

#include <boost/noncopyable.hpp>

#include "vulkan.h"

namespace yaga
{
namespace vk
{

class Buffer : private boost::noncopyable
{
public:
  Buffer(VmaAllocator allocator, const VkBufferCreateInfo& info, const VmaAllocationCreateInfo& allocInfo);
  ~Buffer();
  VkBuffer operator*() const { return buffer_; }
  const VmaAllocation& allocation() const { return allocation_; }
  const VmaAllocationInfo& memory() const { return memory_; }
private:
  VmaAllocator allocator_;
  VkBuffer buffer_;
  VmaAllocation allocation_;
  VmaAllocationInfo memory_;
};

typedef std::unique_ptr<Buffer> BufferPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_BUFFER

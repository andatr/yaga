#ifndef YAGA_VULKAN_RENDERER_BUFFER
#define YAGA_VULKAN_RENDERER_BUFFER

#include "utility/compiler.h"

#include <memory>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "vulkan_renderer/vulkan.h"

namespace yaga {
namespace vk {

class Buffer : private boost::noncopyable
{
public:
  explicit Buffer(VmaAllocator allocator, const VkBufferCreateInfo& info, const VmaAllocationCreateInfo& allocInfo);
  ~Buffer();
  void update(const VkBufferCreateInfo& info, const VmaAllocationCreateInfo& allocInfo);
  VkBuffer operator*() const { return buffer_; }
  const VmaAllocation& allocation() const { return allocation_; }
  const VmaAllocationInfo& memory() const { return memory_;     }

private:
  void clear();

private:
  VmaAllocator allocator_;
  VkBuffer buffer_;
  VmaAllocation allocation_;
  VmaAllocationInfo memory_;
};

typedef std::unique_ptr<Buffer> BufferPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_BUFFER
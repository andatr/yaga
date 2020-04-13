#ifndef YAGA_VULKAN_RENDERER_SRC_ALLOCATOR
#define YAGA_VULKAN_RENDERER_SRC_ALLOCATOR

#include <memory>

#include <boost/noncopyable.hpp>

#include "device.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class Allocator : private boost::noncopyable
{
public:
  explicit Allocator(Device* device);
  AutoDestructor<VkDeviceMemory> allocate(VkBuffer buffer, VkMemoryPropertyFlags properties) const;
  AutoDestructor<VkDeviceMemory> allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties) const;
private:
  Device* device_;
  VkDevice vkDevice;
};

typedef std::unique_ptr<Allocator> AllocatorPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_ALLOCATOR

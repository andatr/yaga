#ifndef YAGA_RENDERER_SRC_ALLOCATOR
#define YAGA_RENDERER_SRC_ALLOCATOR

#include <memory>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "utility/auto_destructor.h"

namespace yaga
{

class Allocator : private boost::noncopyable
{
public:
  explicit Allocator(Device* deivce);
  AutoDestructor<VkDeviceMemory> allocate(VkBuffer buffer, VkMemoryPropertyFlags properties) const;
  AutoDestructor<VkDeviceMemory> allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties) const;
  AutoDestructor<VkDeviceMemory> allocateImage(VkImage image, VkMemoryPropertyFlags properties) const;  
private:
  Device* device_;
  VkDevice vkDevice;
};

typedef std::unique_ptr<Allocator> AllocatorPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_ALLOCATOR

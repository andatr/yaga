#ifndef YAGA_RENDERER_SRC_ALLOCATOR
#define YAGA_RENDERER_SRC_ALLOCATOR

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class Allocator : private boost::noncopyable
{
public:
  explicit Allocator(Device* deivce);
  AutoDestroyer<VkDeviceMemory> Allocate(VkBuffer buffer, VkMemoryPropertyFlags properties) const;
private:
  Device* device_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_ALLOCATOR

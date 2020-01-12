#ifndef YAGA_RENDERER_DEVICE_BUFFER
#define YAGA_RENDERER_DEVICE_BUFFER

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device.h"
#include "utility/auto_destructor.h"

namespace yaga
{

class DeviceBuffer : private boost::noncopyable
{
public:
  DeviceBuffer(Device* device, Allocator* allocator, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryUsage);
  VkBuffer operator*() const { return *buffer_; }
  VkDeviceMemory memory() const { return *memory_; }
  void update(const void* data, size_t size) const;
private:
  Device* device_;
  VkDevice vkDevice_;
  Allocator* allocator_;  
  AutoDestructor<VkBuffer> buffer_;
  AutoDestructor<VkDeviceMemory> memory_;
};

typedef std::unique_ptr<DeviceBuffer> DeviceBufferPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_DEVICE_BUFFER

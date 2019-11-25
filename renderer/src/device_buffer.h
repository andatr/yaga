#ifndef YAGA_RENDERER_DEVICE_BUFFER
#define YAGA_RENDERER_DEVICE_BUFFER

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class DeviceBuffer : private boost::noncopyable
{
public:
  DeviceBuffer(VkDevice device, Allocator* allocator, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryUsage);
  VkBuffer Buffer() const { return *buffer_; }
  VkDeviceMemory Memory() const { return *memory_; }
  void Update(const void* data, size_t size) const;
private:
  VkDevice device_;
  AutoDestroyer<VkBuffer> buffer_;
  AutoDestroyer<VkDeviceMemory> memory_;
};

typedef std::unique_ptr<DeviceBuffer> DeviceBufferPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_DEVICE_BUFFER

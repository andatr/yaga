#ifndef YAGA_RENDERER_UNIFORM_BUFFER
#define YAGA_RENDERER_UNIFORM_BUFFER

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include <vector>

#include "device_buffer.h"
#include "video_buffer.h"

namespace yaga
{

class UniformBuffer : private boost::noncopyable
{
public:
  UniformBuffer(VkDevice device, Allocator* allocator, VideoBuffer* videoBuffer);
  void Update(uint32_t index);
private:
  VideoBuffer* videoBuffer_;
  std::vector<DeviceBufferPtr> buffers_;
};

typedef std::unique_ptr<UniformBuffer> UniformBufferPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_UNIFORM_BUFFER

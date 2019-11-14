#ifndef YAGA_RENDERER_MODEL
#define YAGA_RENDERER_MODEL

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "material.h"
#include "mesh.h"
#include "video_buffer.h"

namespace yaga
{

class Model : private boost::noncopyable
{
public:
  Model(Device* device, VideoBuffer* videoBuffer, Mesh* mesh, Material* material);
  const std::vector<VkCommandBuffer>& CommandBuffers() const { return commandBuffers_; }
private:
  void CreateCommandBuffer(Device* device, VideoBuffer* videoBuffer);
private:
  Mesh* mesh_;
  Material* material_;
  std::vector<VkCommandBuffer> commandBuffers_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_MODEL

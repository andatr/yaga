#ifndef YAGA_RENDERER_MODEL
#define YAGA_RENDERER_MODEL

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "material.h"
#include "mesh.h"

namespace yaga
{

class Model : private boost::noncopyable
{
public:
  Model(std::unique_ptr<Material> material, std::unique_ptr<Mesh> mesh, VkDevice device, VkCommandPool commandPool, VideoBuffer* videoBuffer);
  const std::vector<VkCommandBuffer>& CommandBuffers() const { return commandBuffers_; }
private:
  void CreateCommandBuffer(VkDevice device, VkCommandPool commandPool, VideoBuffer* videoBuffer);
private:
  std::unique_ptr<Material> material_;
  std::unique_ptr<Mesh> mesh_;
  std::vector<VkCommandBuffer> commandBuffers_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_MODEL

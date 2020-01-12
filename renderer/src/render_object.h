#ifndef YAGA_RENDERER_MODEL
#define YAGA_RENDERER_MODEL

#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "material.h"
#include "mesh.h"
#include "swapchain.h"

namespace yaga
{

class RenderObject : private boost::noncopyable
{
public:
  RenderObject(Device* device, Swapchain* swapchain, Mesh* mesh, Material* material, const std::vector<ImageView*>& textures);
  const std::vector<VkCommandBuffer>& commandBuffers() const { return commandBuffers_; }
private:
  void createDescriptorSets();
  void createCommandBuffer();
private:
  Device* device_;
  VkDevice vkDevice_;
  Swapchain* swapchain_;
  Mesh* mesh_;
  Material* material_;
  std::vector<ImageView*> textures_;
  std::vector<VkDescriptorSet> descriptorSets_;
  std::vector<VkCommandBuffer> commandBuffers_;
};

typedef std::unique_ptr<RenderObject> RenderObjectPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_MODEL

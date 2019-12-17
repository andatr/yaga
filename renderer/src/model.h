#ifndef YAGA_RENDERER_MODEL
#define YAGA_RENDERER_MODEL

#include <memory>
#include <vector>

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
  Model(Device* device, VideoBuffer* videoBuffer, Mesh* mesh, Material* material, const std::vector<ImageView*>& textures);
  const std::vector<VkCommandBuffer>& CommandBuffers() const { return commandBuffers_; }
private:
  void Model::CreateDescriptorSets(VkDevice device, VideoBuffer* videoBuffer);
  void CreateCommandBuffer(Device* device, VideoBuffer* videoBuffer);
private:
  VkDevice device_;
  VideoBuffer* videoBuffer_;
  Mesh* mesh_;
  Material* material_;
  std::vector<ImageView*> textures_;
  std::vector<VkDescriptorSet> descriptorSets_;
  std::vector<VkCommandBuffer> commandBuffers_;
};

typedef std::unique_ptr<Model> ModelPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_MODEL

#ifndef YAGA_VULKAN_RENDERER_MATERIAL
#define YAGA_VULKAN_RENDERER_MATERIAL

#include <memory>

#include "vulkan_renderer/vulkan.h"
#include "engine/material.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class MaterialPool;

struct Pipeline
{
  AutoDestructor<VkPipeline> main;
  AutoDestructor<VkPipeline> wireframe;
  std::vector<VkDescriptorSet> descriptors;
};

typedef std::unique_ptr<Pipeline> PipelinePtr;

class Material : public yaga::Material
{
public:
  explicit Material(MaterialPool* pool, assets::MaterialPtr asset, Pipeline* pipeline);
  virtual ~Material();
  bool wireframe() override { return wireframe_; }
  void wireframe(bool value) override { wireframe_ = value; }
  const Pipeline& pipeline() { return *pipeline_; }

private:
  MaterialPool* pool_;
  Pipeline* pipeline_;
  bool wireframe_;
};

typedef std::unique_ptr<Material> MaterialPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_MATERIAL

#ifndef YAGA_VULKAN_RENDERER_SRC_MATERIAL
#define YAGA_VULKAN_RENDERER_SRC_MATERIAL

#include <memory>
#include <vector>

#include "vulkan.h"
#include "engine/material.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class Material : public yaga::Material
{
public:
  explicit Material(VkPipeline pipeline, VkPipelineLayout layout, const std::vector<VkDescriptorSet>& descriptors);
  VkPipelineLayout pipelineLayout() const { return layout_; }
  VkPipeline pipeline() const { return pipeline_; }
  const std::vector<VkDescriptorSet>& descriptorSets() const { return descriptorSets_; }
private:
  friend class MaterialPool;
private:
  VkPipeline pipeline_;
  VkPipelineLayout layout_;
  std::vector<VkDescriptorSet> descriptorSets_;
};

typedef std::unique_ptr<Material> MaterialPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_MATERIAL

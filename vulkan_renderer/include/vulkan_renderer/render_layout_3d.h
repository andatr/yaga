#ifndef YAGA_VULKAN_RENDERER_RENDER_LAYOUT_3D
#define YAGA_VULKAN_RENDERER_RENDER_LAYOUT_3D

#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class RenderLayout3D
{
public:
  explicit RenderLayout3D(VkDevice device);
  VkDescriptorSetLayout uniform() const { return *uniform_;  }
  VkDescriptorSetLayout texture() const { return *texture_;  }
  VkPipelineLayout     pipeline() const { return *pipeline_; }

private:
  void createUniform (VkDevice device);
  void createTexture (VkDevice device);
  void createPipeline(VkDevice device);

private:
  AutoDestructor<VkDescriptorSetLayout> uniform_;
  AutoDestructor<VkDescriptorSetLayout> texture_;
  AutoDestructor<VkPipelineLayout>      pipeline_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_RENDER_LAYOUT_3D

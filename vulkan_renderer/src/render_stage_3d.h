#ifndef YAGA_VULKAN_RENDERER_SRC_RENDER_STAGE_3D
#define YAGA_VULKAN_RENDERER_SRC_RENDER_STAGE_3D

#include "material.h"
#include "render_pass.h"
#include "render_stage.h"
#include "swapchain.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class RenderStage3D : public RenderStage
{
public:
  explicit RenderStage3D(Swapchain* swapchain, VmaAllocator allocator, RenderPassPtr renderPass);
  ~RenderStage3D();
  VkSemaphore render(Context* context, uint32_t frame, VkSemaphore waitFor) override;

private:
  struct Uniform
  {
    BufferPtr buffer;
    VkDescriptorSet descriptor;
  };

private:
  void createUniform();
  void updateUniform(uint32_t frame, Context* context);
  void renderObject(Renderer3D* object, VkCommandBuffer command, uint32_t frame);

private:
  Swapchain* swapchain_;
  VmaAllocator allocator_;
  RenderPassPtr renderPass_;
  std::vector<Uniform> uniform_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDER_STAGE_3D

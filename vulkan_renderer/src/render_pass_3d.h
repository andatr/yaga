#ifndef YAGA_VULKAN_RENDERER_SRC_RENDER_PASS_3D
#define YAGA_VULKAN_RENDERER_SRC_RENDER_PASS_3D

#include "render_pass.h"
#include "swapchain.h"

namespace yaga {
namespace vk {

class RenderPass3D : public RenderPass
{
public:
  explicit RenderPass3D(Swapchain* swapchain);
  virtual ~RenderPass3D();
  VkCommandBuffer beginRender(uint32_t image) override;

private:
  typedef RenderPass base;

private:
  void createRenderPass();
  void createFrameBuffers();
  void createUniformLayout();
  void createTextureLayout();
  void createPipelineLayout();
  void createSampler();
  void onResize();

private:
  Swapchain* swapchain_;
  Swapchain::Connection resizeConnection_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDER_PASS_3D

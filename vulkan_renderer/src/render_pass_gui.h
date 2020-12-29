#ifndef YAGA_VULKAN_RENDERER_SRC_RENDER_PASS_GUI
#define YAGA_VULKAN_RENDERER_SRC_RENDER_PASS_GUI

#include "render_pass.h"
#include "swapchain.h"

namespace yaga {
namespace vk {

class RenderPassGui : public RenderPass
{
public:
  explicit RenderPassGui(Swapchain* swapchain);
  virtual ~RenderPassGui();

private:
  typedef RenderPass base;

private:
  void createRenderPass();
  void createFrameBuffers();
  void onResize();

private:
  Swapchain* swapchain_;
  Swapchain::Connection resizeConnection_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDER_PASS_GUI

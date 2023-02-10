#ifndef YAGA_VULKAN_RENDERER_RENDER_PASS
#define YAGA_VULKAN_RENDERER_RENDER_PASS

#include <memory>
#include <vector>

#include "vulkan_renderer/frame_buffer.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"

namespace yaga {
namespace vk {

class IRenderPass
{
public:
  virtual ~IRenderPass() {}
  virtual VkSemaphore render(uint32_t imageIndex, VkSemaphore waitFor) = 0;
};

typedef std::shared_ptr<IRenderPass> RenderPassPtr;

class RenderPass : public IRenderPass
{
public:
  explicit RenderPass(Swapchain* swapchain);
  virtual ~RenderPass();
  Swapchain*   swapchain() { return swapchain_;   }
  VkRect2D    renderArea() { return renderArea_;  }
  VkRenderPass operator*() { return *renderPass_; }
  VkSemaphore render(uint32_t imageIndex, VkSemaphore waitFor) override;

protected:
  virtual void render(uint32_t imageIndex) = 0;
  virtual void beginRender(uint32_t imageIndex);
  virtual VkSemaphore finishRender(uint32_t imageIndex, VkSemaphore waitFor);
  virtual void onResize();

protected:
  Swapchain* swapchain_;
  VkRect2D renderArea_;
  std::vector<VkClearValue> clearValues_;
  AutoDestructor<VkRenderPass> renderPass_;
  std::vector<FrameBufferPtr> frameBuffers_;
  SignalConnections connections_;
};

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_RENDER_PASS

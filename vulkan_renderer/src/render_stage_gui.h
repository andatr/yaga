#ifndef YAGA_VULKAN_RENDERER_SRC_RENDER_STAGE_GUI
#define YAGA_VULKAN_RENDERER_SRC_RENDER_STAGE_GUI

#include "config.h"
#include "context.h"
#include "device.h"
#include "render_stage.h"
#include "swapchain.h"
#include "vulkan.h"
#include "window.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class RenderStageGui : public RenderStage
{
public:
  explicit RenderStageGui(Swapchain* swapchain, Window* window, const Config& config);
  virtual ~RenderStageGui();
  VkSemaphore render(Context* context, uint32_t frame, VkSemaphore waitFor) override;

private:
  void createDescriptorPool();
  void initGui(Window* window, const Config& config);
  void uploadFonts();

private:
  Swapchain* swapchain_;
  RenderPassPtr renderPass_;
  AutoDestructor<VkDescriptorPool> descriptorPool_;
};

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDER_STAGE_GUI

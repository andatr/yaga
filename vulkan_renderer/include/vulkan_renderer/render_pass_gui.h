#ifndef YAGA_VULKAN_RENDERER_RENDER_PASS_GUI
#define YAGA_VULKAN_RENDERER_RENDER_PASS_GUI

#include <functional>

#include "vulkan_renderer/render_pass.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"
#include "vulkan_renderer/window.h"
#include "engine/config.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class RenderPassGui : public RenderPass
{
public:
  typedef std::function<void()> GuiProc;

public:
  RenderPassGui(Swapchain* swapchain, Window* window, ConfigPtr config, GuiProc proc);
  virtual ~RenderPassGui();
  void saveConfig();

protected:
  void beginRender(uint32_t imageIndex) override;
  void render(uint32_t imageIndex) override;
  void onResize() override;

private:
  typedef RenderPass base;

private:
  void createDescriptorPool();
  void createRenderPass();
  void createFrameBuffers();
  void initGui(Window* window);
  void uploadFonts();

private:
  ConfigPtr config_;
  AutoDestructor<VkDescriptorPool> descriptorPool_;
  GuiProc proc_;
};

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_RENDER_PASS_GUI

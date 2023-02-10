#ifndef YAGA_VULKAN_PLAYER_SRC_GUI_RENDER_PASS
#define YAGA_VULKAN_PLAYER_SRC_GUI_RENDER_PASS

#include <vector>

#include "utility/auto_destructor.h"
#include "vulkan_renderer/buffer.h"
#include "vulkan_renderer/render_pass.h"

namespace yaga {
namespace vk {

class GuiRenderPass : public RenderPass
{
public:
  GuiRenderPass(Swapchain* swapchain, VmaAllocator allocator);

protected:
  void render(uint32_t imageIndex) override;
  void onResize() override;

private:
  typedef RenderPass base;

private:
  void createPipelineLayout();
  void createRenderPass();
  void createShaders();
  void createPipeline();
  void createFrameBuffers();
  void updateFrameBuffers();
  void createBuffers();
  void updateVertices();

private:
  VmaAllocator allocator_;
  AutoDestructor<VkPipelineLayout> pipelineLayout_;
  AutoDestructor<VkShaderModule> vertexShader_;
  AutoDestructor<VkShaderModule> fragmentShader_;
  AutoDestructor<VkPipeline> pipeline_;
  BufferPtr vertexBuffer_;
  BufferPtr indexBuffer_;
  BufferPtr stageBuffer_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_GUI_RENDER_PASS

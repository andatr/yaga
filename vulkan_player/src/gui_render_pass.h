#ifndef YAGA_VULKAN_PLAYER_SRC_GUI_RENDER_PASS
#define YAGA_VULKAN_PLAYER_SRC_GUI_RENDER_PASS

#include <vector>

#include "engine/input.h"
#include "utility/auto_destructor.h"
#include "vulkan_renderer/buffer.h"
#include "vulkan_renderer/render_pass.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"

namespace yaga {
namespace vk {

struct Vertex
{
  glm::vec3 position;
};

class GuiRenderPass : public RenderPass
{
public:
  GuiRenderPass(Swapchain* swapchain, VmaAllocator allocator, Input* input);

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
  Input* input_;
  AutoDestructor<VkPipelineLayout> pipelineLayout_;
  AutoDestructor<VkShaderModule> vertexShader_;
  AutoDestructor<VkShaderModule> fragmentShader_;
  AutoDestructor<VkPipeline> pipeline_;
  BufferPtr vertexBuffer_;
  BufferPtr indexBuffer_;
  BufferPtr stageBuffer_;
  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;

  bool mouseDown_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_GUI_RENDER_PASS

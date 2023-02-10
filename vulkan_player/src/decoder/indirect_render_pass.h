#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_INDIRECT_RENDER_PASS
#define YAGA_VULKAN_PLAYER_SRC_DECODER_INDIRECT_RENDER_PASS

#include "decoder_frontend.h"
#include "decoder_middleware.h"
#include "utility/auto_destructor.h"
#include "vulkan_renderer/render_pass.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"

namespace yaga {
namespace vk {

class IndirectRenderPass : public RenderPass
{
public:
  IndirectRenderPass(Swapchain* swapchain, VmaAllocator allocator, DecoderMiddlewarePtr decoder);

protected:
  void render(uint32_t imageIndex) override;
  void onResize() override;

private:
  typedef RenderPass base;

private:
  void createDescriptorPool();
  void createDescriptorLayout();
  void createPipelineLayout();
  void createRenderPass();
  void createShaders();
  void createPipeline();
  void createSampler();
  void createImage();
  void createDescriptor();
  void updateDescriptor();
  void createFrameBuffers();
  void updateFrameBuffers();

private:
  VmaAllocator allocator_;
  DecoderFrontendPtr decoder_;
  AutoDestructor<VkDescriptorPool> descriptorPool_;
  AutoDestructor<VkDescriptorSetLayout> descriptorLayout_;
  AutoDestructor<VkPipelineLayout> pipelineLayout_;
  AutoDestructor<VkShaderModule> vertexShader_;
  AutoDestructor<VkShaderModule> fragmentShader_;
  AutoDestructor<VkPipeline> pipeline_;
  ImagePtr image_;
  VkDescriptorSet imageDescriptor_;
  AutoDestructor<VkSampler> sampler_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_INDIRECT_RENDER_PASS

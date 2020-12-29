#ifndef YAGA_VULKAN_RENDERER_SRC_RENDER_PASS
#define YAGA_VULKAN_RENDERER_SRC_RENDER_PASS

#include <functional>
#include <memory>

#include "device.h"
#include "frame_buffer.h"
#include "swapchain.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

//class IRenderPassRender

// -----------------------------------------------------------------------------------------------------------------------------
class RenderPass
{
public:
  struct WaitFor
  {
    VkPipelineStageFlags stage;
    VkSemaphore semaphore;
  };

public:
  explicit RenderPass(Device* device);
  virtual ~RenderPass();
  VkRenderPass operator*() const { return *renderPass_; }
  Device*         device() const { return device_;      }
  VkRect2D    renderArea() const { return renderArea_;  }
  VkDescriptorSetLayout uniformLayout() const { return *uniformLayout_;  }
  VkDescriptorSetLayout textureLayout() const { return *textureLayout_;  }
  VkPipelineLayout     pipelineLayout() const { return *pipelineLayout_; }
  VkSampler                   sampler() const { return *sampler_;        }
  VkPipelineStageFlags     finalStage() const { return finalStage_;      }
  virtual VkCommandBuffer beginRender(uint32_t image);
  virtual WaitFor finishRender(uint32_t image, WaitFor waitFor);

protected:
  Device* device_;
  VkRect2D renderArea_;
  VkPipelineStageFlags finalStage_;
  std::vector<VkClearValue> clearValues_;
  AutoDestructor<VkRenderPass> renderPass_;
  AutoDestructor<VkDescriptorSetLayout> uniformLayout_;
  AutoDestructor<VkDescriptorSetLayout> textureLayout_;
  AutoDestructor<VkPipelineLayout> pipelineLayout_;
  AutoDestructor<VkSampler> sampler_;
  std::vector<FrameBufferPtr> frameBuffers_;
};

typedef std::unique_ptr<RenderPass> RenderPassPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDER_PASS

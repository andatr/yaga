#ifndef YAGA_VULKAN_RENDERER_FRAME_BUFFER
#define YAGA_VULKAN_RENDERER_FRAME_BUFFER

#include <memory>

#include "vulkan_renderer/buffer.h"
#include "vulkan_renderer/device.h"
#include "vulkan_renderer/vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class RenderPass;

class FrameBuffer
{
public:
  explicit FrameBuffer(RenderPass* renderPass, VkImageView* attachments, size_t attachmentCount);
  virtual ~FrameBuffer();
  void update(VkImageView* attachments, size_t attachmentCount);
  VkFramebuffer   operator*()         const { return *frameBuffer_;        }
  VkCommandBuffer command()           const { return *command_;            }
  VkFence         syncSubmit()        const { return *syncSubmit_;         }
  VkSemaphore     syncRender()        const { return *syncRender_;         }

private:
  void createFrameBuffer(VkImageView* attachments, size_t attachmentCount);
  void createCommand(Device* device);

private:
  RenderPass* renderPass_;
  AutoDestructor<VkFramebuffer> frameBuffer_;
  AutoDestructor<VkFence> syncSubmit_;
  AutoDestructor<VkSemaphore> syncRender_;
  AutoDestructor<VkCommandBuffer> command_;
};

typedef std::unique_ptr<FrameBuffer> FrameBufferPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_FRAME_BUFFER

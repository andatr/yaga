#ifndef YAGA_VULKAN_RENDERER_SRC_FRAME_BUFFER
#define YAGA_VULKAN_RENDERER_SRC_FRAME_BUFFER

#include <memory>

#include "buffer.h"
#include "device.h"
#include "vulkan.h"
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
  VkCommandBuffer command()           const { return command_;             }
  VkFence         syncSubmit()        const { return *syncSubmit_;         }
  VkSemaphore     syncRender()        const { return *syncRender_;         }

private:
  void createFrameBuffer(RenderPass* renderPass, VkImageView* attachments, size_t attachmentCount);
  void createFence(VkDevice device);
  void createSemaphore(VkDevice device);
  void createCommand(Device* device);

private:
  RenderPass* renderPass_;
  AutoDestructor<VkFramebuffer> frameBuffer_;
  AutoDestructor<VkFence> syncSubmit_;
  AutoDestructor<VkSemaphore> syncRender_;
  VkCommandBuffer command_;
};

typedef std::unique_ptr<FrameBuffer> FrameBufferPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_FRAME_BUFFER

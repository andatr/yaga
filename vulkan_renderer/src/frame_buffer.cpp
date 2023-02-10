#include "precompiled.h"
#include "vulkan_renderer/frame_buffer.h"
#include "vulkan_renderer/render_pass.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
FrameBuffer::FrameBuffer(RenderPass* renderPass, VkImageView* attachments, size_t attachmentCount) :
  renderPass_(renderPass)
{
  auto device = renderPass_->swapchain()->device();
  syncRender_ = createSemaphore(**device);
  syncSubmit_ = createFence(**device);
  createFrameBuffer(attachments, attachmentCount);
  createCommand(device);
}

// -----------------------------------------------------------------------------------------------------------------------------
FrameBuffer::~FrameBuffer()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameBuffer::update(VkImageView* attachments, size_t attachmentCount)
{
  createFrameBuffer(attachments, attachmentCount);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameBuffer::createFrameBuffer(VkImageView* attachments, size_t attachmentCount)
{
  auto device = **renderPass_->swapchain()->device();
  auto destroyFrameBuffer = [device](auto frameBuffer) {
    vkDestroyFramebuffer(device, frameBuffer, nullptr);
    LOG(trace) << "Framebuffer destroyed";
  };
  VkFramebuffer frameBuffer;
  VkFramebufferCreateInfo info{};
  info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.renderPass      = **renderPass_;
  info.pAttachments    = attachments;
  info.attachmentCount = static_cast<uint32_t>(attachmentCount);
  info.width           = renderPass_->renderArea().extent.width;
  info.height          = renderPass_->renderArea().extent.height;
  info.layers          = 1;
  VULKAN_GUARD(vkCreateFramebuffer(device, &info, nullptr, &frameBuffer), "Could not create Framebuffer");
  frameBuffer_.set(frameBuffer, destroyFrameBuffer);
  LOG(trace) << "Framebuffer created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameBuffer::createCommand(Device* device)
{
  VkCommandBufferAllocateInfo info{};
  info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.commandPool        = device->commandPool();
  info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  info.commandBufferCount = 1;
  command_ = vk::createCommand(**device, device->commandPool(), info);
}

} // !namespace vk
} // !namespace yaga
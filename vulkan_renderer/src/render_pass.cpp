#include "precompiled.h"
#include "render_pass.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
RenderPass::RenderPass(Device* device) :
  device_(device),
  finalStage_(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
RenderPass::~RenderPass()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
VkCommandBuffer RenderPass::beginRender(uint32_t image)
{
  const auto& frame = frameBuffers_[image];

  VkRenderPassBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  info.renderPass      = *renderPass_;
  info.renderArea      = renderArea_;
  info.clearValueCount = static_cast<uint32_t>(clearValues_.size());
  info.pClearValues    = clearValues_.data();
  info.framebuffer     = **frame;

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  const auto fence = frame->syncSubmit();
  vkWaitForFences(**device_, 1, &fence, VK_TRUE, UINT64_MAX);
  const auto command = frame->command();
  VULKAN_GUARD(vkBeginCommandBuffer(command, &beginInfo), "Failed to start recording Command Buffer");
  vkCmdBeginRenderPass(command, &info, VK_SUBPASS_CONTENTS_INLINE);
  return command;
}

// -----------------------------------------------------------------------------------------------------------------------------
RenderPass::WaitFor RenderPass::finishRender(uint32_t image, WaitFor waitFor)
{
  const auto& frame     = frameBuffers_[image];
  const auto  fence     = frame->syncSubmit();
  const auto  semaphore = frame->syncRender();
  const auto  command   = frame->command();

  vkCmdEndRenderPass(command);
  VULKAN_GUARD(vkEndCommandBuffer(command), "Failed to record Command Buffer");

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount   = 1;
  submitInfo.pWaitSemaphores      = &waitFor.semaphore;
  submitInfo.pWaitDstStageMask    = &waitFor.stage;
  submitInfo.commandBufferCount   = 1;
  submitInfo.pCommandBuffers      = &command;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores    = &semaphore;
  vkResetFences(**device_, 1, &fence);
  VULKAN_GUARD(vkQueueSubmit(device_->graphicsQueue(), 1, &submitInfo, fence), "Could not draw frame");

  return { finalStage_, semaphore };
}

} // !namespace vk
} // !namespace yaga
#include "precompiled.h"
#include "vulkan_renderer/render_pass.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
inline VkRect2D getRenderArea(Swapchain* swapchain)
{
  const auto& r = swapchain->resolution();
  return {{ 0, 0 }, { r.width, r.height }};
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
RenderPass::RenderPass(Swapchain* swapchain) :
  swapchain_(swapchain),
  renderArea_(getRenderArea(swapchain))
{
  connections_.push_back(swapchain_->onResize([this]() { onResize(); }));
}

// -----------------------------------------------------------------------------------------------------------------------------
RenderPass::~RenderPass()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass::onResize()
{
  renderArea_ = getRenderArea(swapchain_);
}

// -----------------------------------------------------------------------------------------------------------------------------
VkSemaphore RenderPass::render(uint32_t imageIndex, VkSemaphore waitFor)
{
  beginRender(imageIndex);
  render(imageIndex);
  return finishRender(imageIndex, waitFor);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass::beginRender(uint32_t imageIndex)
{
  const auto* frame = frameBuffers_[imageIndex].get();

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
  vkWaitForFences(**swapchain_->device(), 1, &fence, VK_TRUE, UINT64_MAX);
  const auto command = frame->command();
  VULKAN_GUARD(vkBeginCommandBuffer(command, &beginInfo), "Failed to start recording Command Buffer");
  vkCmdBeginRenderPass(command, &info, VK_SUBPASS_CONTENTS_INLINE);
}

// -----------------------------------------------------------------------------------------------------------------------------
VkSemaphore RenderPass::finishRender(uint32_t imageIndex, VkSemaphore waitFor)
{
  const auto* frame     = frameBuffers_[imageIndex].get();
  const auto  fence     = frame->syncSubmit();
  const auto  semaphore = frame->syncRender();
  const auto  command   = frame->command();

  vkCmdEndRenderPass(command);
  VULKAN_GUARD(vkEndCommandBuffer(command), "Failed to record Command Buffer");

  const VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount   = 1;
  submitInfo.pWaitSemaphores      = &waitFor;
  submitInfo.pWaitDstStageMask    = &stage;
  submitInfo.commandBufferCount   = 1;
  submitInfo.pCommandBuffers      = &command;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores    = &semaphore;
  vkResetFences(**swapchain_->device(), 1, &fence);
  VULKAN_GUARD(vkQueueSubmit(swapchain_->device()->graphicsQueue(), 1, &submitInfo, fence), "Could not draw a frame");
  return semaphore;
}

} // !namespace vk
} // !namespace yaga
#include "precompiled.h"
#include "direct_render_pass.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
DirectRenderPass::DirectRenderPass(Swapchain* swapchain, VmaAllocator allocator, DecoderMiddlewarePtr decoder) :
  swapchain_(swapchain)
{
  decoder_ = createDecoderFrontend(swapchain, allocator, swapchain_->images(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, decoder);
  connections_.push_back(swapchain->onResize([this]() { onResize(); }));
}

// -----------------------------------------------------------------------------------------------------------------------------
VkSemaphore DirectRenderPass::render(uint32_t imageIndex, VkSemaphore waitFor)
{
  decoder_->decode(imageIndex);
  return waitFor;
}

// -----------------------------------------------------------------------------------------------------------------------------
void DirectRenderPass::onResize()
{
  decoder_->resize(swapchain_->images(), swapchain_->resolution());
}

} // !namespace vk
} // !namespace yaga
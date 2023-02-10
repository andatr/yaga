#include "precompiled.h"
#include "decoder_frontend.h"
#include "decoder_render_pass.h"
#include "direct_render_pass.h"
#include "indirect_render_pass.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
RenderPassPtr createDecoderRenderPass(Swapchain* swapchain, VmaAllocator allocator, DecoderMiddlewarePtr decoder)
{
  if (swapchain->capabilitues().supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) {
    return std::make_unique<DirectRenderPass>(swapchain, allocator, decoder);
  }
  return std::make_unique<IndirectRenderPass>(swapchain, allocator, decoder);
}

} // !namespace vk
} // !namespace yaga
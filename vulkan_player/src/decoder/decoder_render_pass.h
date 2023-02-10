#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_RENDER_PASS
#define YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_RENDER_PASS

#include "decoder_middleware.h"
#include "vulkan_renderer/render_pass.h"
#include "vulkan_renderer/swapchain.h"

namespace yaga {
namespace vk {

RenderPassPtr createDecoderRenderPass(Swapchain* swapchain, VmaAllocator allocator, DecoderMiddlewarePtr decoder);

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_RENDER_PASS

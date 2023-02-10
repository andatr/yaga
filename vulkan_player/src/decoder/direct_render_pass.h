#ifndef YAGA_VULKAN_PLAYER_SRC_DIRECT_RENDER_PASS
#define YAGA_VULKAN_PLAYER_SRC_DIRECT_RENDER_PASS

#include "decoder_frontend.h"
#include "decoder_middleware.h"
#include "utility/auto_destructor.h"
#include "vulkan_renderer/render_pass.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"

namespace yaga {
namespace vk {

class DirectRenderPass : public IRenderPass
{
public:
  DirectRenderPass(Swapchain* swapchain, VmaAllocator allocator, DecoderMiddlewarePtr decoder);
  VkSemaphore render(uint32_t imageIndex, VkSemaphore waitFor) override;

private:
  void onResize();

private:
  Swapchain* swapchain_;
  DecoderFrontendPtr decoder_;
  SignalConnections connections_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DIRECT_RENDER_PASS

#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_FRONTEND
#define YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_FRONTEND

#include <memory>
#include <vector>

#include "decoder_middleware.h"
#include "vulkan_renderer/image_view.h"
#include "vulkan_renderer/swapchain.h"

namespace yaga {
namespace vk {

class DecoderFrontend
{
public:
  virtual ~DecoderFrontend() {}
  virtual void decode(int index) = 0;
  virtual void resize(const std::vector<ImageView>& images, const VkExtent2D& size) = 0;
};

typedef std::unique_ptr<DecoderFrontend> DecoderFrontendPtr;

DecoderFrontendPtr createDecoderFrontend(
  Swapchain* swapchain,
  VmaAllocator allocator,
  const std::vector<ImageView>& images,
  VkImageLayout finalLayout,
  DecoderMiddlewarePtr decoder);

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_FRONTEND

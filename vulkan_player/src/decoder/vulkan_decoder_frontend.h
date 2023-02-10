#ifndef YAGA_VULKAN_PLAYER_SRC_VULKAN_DECODER_DECOODER_FRONTEND
#define YAGA_VULKAN_PLAYER_SRC_VULKAN_DECODER_DECOODER_FRONTEND

#include "color_space_converter.h"
#include "decoder_frontend.h"
#include "frame_uploader.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"

namespace yaga {
namespace vk {

class VulkanDecoderFrontend: public DecoderFrontend
{
public:
  VulkanDecoderFrontend(
    Swapchain* swapchain,
    VmaAllocator allocator,
    const std::vector<ImageView>& images,
    VkImageLayout finalLayout,
    DecoderMiddlewarePtr decoder);
  virtual ~VulkanDecoderFrontend() {}
  void decode(int index) override;
  void resize(const std::vector<ImageView>& images, const VkExtent2D& size) override;

private:
  void changeFormat(Frame* frame);

private:
  DecoderMiddlewarePtr decoder_;
  FrameUploaderPtr frameUploader_;
  ColorSpaceConverterPtr colorConverter_;
  std::vector<ColorSpaceConverter::Images> images_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_VULKAN_DECODER_DECOODER_FRONTEND

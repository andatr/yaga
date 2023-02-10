#include "precompiled.h"
#include "vulkan_decoder_frontend.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
DecoderFrontendPtr createDecoderFrontend(
  Swapchain* swapchain,
  VmaAllocator allocator,
  const std::vector<ImageView>& images,
  VkImageLayout finalLayout,
  DecoderMiddlewarePtr decoder)
{
  return std::make_unique<VulkanDecoderFrontend>(swapchain, allocator, images, finalLayout, decoder);
}

// -----------------------------------------------------------------------------------------------------------------------------
VulkanDecoderFrontend::VulkanDecoderFrontend(
  Swapchain* swapchain,
  VmaAllocator allocator,
  const std::vector<ImageView>& images,
  VkImageLayout finalLayout,
  DecoderMiddlewarePtr decoder
) :
  decoder_(decoder),
  images_(images.size())
{
  frameUploader_ = std::make_unique<FrameUploader>(swapchain, allocator);
  for (int i = 0; i < images_.size(); ++i) {
    images_[i].dest = images[i];
  }
  colorConverter_ = std::make_unique<ColorSpaceConverter>(swapchain, allocator, images_.size(), finalLayout);
}

// -----------------------------------------------------------------------------------------------------------------------------
void VulkanDecoderFrontend::decode(int index)
{
  decoder_->getFrame([this, index](Frame* frame) {
    if (!frame) return;
    if (!frameUploader_->upload(frame)) {
      changeFormat(frame);
    }
    frameUploader_->upload(frame);
    colorConverter_->convert(index);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void VulkanDecoderFrontend::resize(const std::vector<ImageView>& images, const VkExtent2D& size)
{
  decoder_->resize(size.width, size.height);
  for (int i = 0; i < images_.size(); ++i) {
    images_[i].dest = images[i];
  }
  colorConverter_->update(images_, frameUploader_->format());
}

// -----------------------------------------------------------------------------------------------------------------------------
void VulkanDecoderFrontend::changeFormat(Frame* frame)
{
  colorConverter_->waitIdle();
  frameUploader_->changeFormat(frame->format());
  for (int i = 0; i < images_.size(); ++i) {
    images_[i].chroma = frameUploader_->chroma();
    images_[i].luma = frameUploader_->luma();
  }
  colorConverter_->update(images_, frameUploader_->format());
}

} // !namespace vk
} // !namespace yaga

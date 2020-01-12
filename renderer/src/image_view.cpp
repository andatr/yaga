#include "precompiled.h"
#include "image_view.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
ImageView::ImageView(Device* device, Image* image, VkImageAspectFlagBits aspectMask) :
  device_(device), vkDevice_(**device), image_(image), aspectMask_(aspectMask)
{
  createImageView();
}

// -------------------------------------------------------------------------------------------------------------------------
void ImageView::createImageView()
{
  VkImageViewCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.image = **image_;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.format = image_->format();
  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.subresourceRange.aspectMask = aspectMask_;
  info.subresourceRange.baseMipLevel = 0;
  info.subresourceRange.levelCount = image_->mipLevels();
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount = 1;

  auto destroyImageView = [device = vkDevice_](auto view) {
    vkDestroyImageView(device, view, nullptr);
    LOG(trace) << "Image View destroyed";
  };
  VkImageView view;
  if (vkCreateImageView(vkDevice_, &info, nullptr, &view) != VK_SUCCESS) {
    THROW("Could not create Image View");
  }
  imageView_.set(view, destroyImageView);
  LOG(trace) << "Image View created";
}

} // !namespace yaga
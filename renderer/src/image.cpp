#include "precompiled.h"
#include "image.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Image::Image(VkImage image, VkFormat format) :
  device_(nullptr), vkDevice_(0), allocator_(nullptr), asset_(nullptr), format_(format), mipLevels_(1)
{
  image_.set(image, [](auto){});
}

// -------------------------------------------------------------------------------------------------------------------------
Image::Image(Device* device, Allocator* allocator, VkExtent2D size, VkFormat format,
  VkSampleCountFlagBits samples, VkImageUsageFlags usage) :
    device_(device), vkDevice_(**device), allocator_(allocator), asset_(nullptr), format_(format), mipLevels_(1)
{
  createImage(size, usage, samples);
}

// -------------------------------------------------------------------------------------------------------------------------
Image::Image(Device* device, Allocator* allocator, asset::Texture* asset) : 
  device_(device), vkDevice_(**device), allocator_(allocator), asset_(asset), format_(VK_FORMAT_R8G8B8A8_UNORM), mipLevels_(1)
{
  mipLevels_ = static_cast<uint32_t>(std::floor(std::log2(std::max(asset->width(), asset->height())))) + 1;
  createImage(
    { static_cast<uint32_t>(asset->width()), static_cast<uint32_t>(asset->height()) },
    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    VK_SAMPLE_COUNT_1_BIT);
  auto buffer = createBuffer();
  changeLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  copyBuffer(**buffer);
  //ChangeLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  generateMipMaps();
}

// -------------------------------------------------------------------------------------------------------------------------
void Image::createImage(VkExtent2D size, VkImageUsageFlags usage, VkSampleCountFlagBits samples)
{
  VkImageCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.extent.width = size.width;
  info.extent.height = size.height;
  info.extent.depth = 1;
  info.mipLevels = mipLevels_;
  info.arrayLayers = 1;
  info.format = format_;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage = usage;
  info.samples = samples;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  auto destroyImage = [device = vkDevice_](auto image) {
    vkDestroyImage(device, image, nullptr);
    LOG(trace) << "Image destroyed";
  };
  VkImage image;
  if (vkCreateImage(vkDevice_, &info, nullptr, &image) != VK_SUCCESS) {
    THROW("Could not create Image");
  }
  image_.set(image, destroyImage);
  memory_ = allocator_->allocateImage(image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkBindImageMemory(vkDevice_, image, *memory_, 0);
}

// -------------------------------------------------------------------------------------------------------------------------
DeviceBufferPtr Image::createBuffer() const
{
  auto stageBuffer = std::make_unique<DeviceBuffer>(device_, allocator_, asset_->Size(),
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stageBuffer->update(asset_->data(), asset_->Size());
  return stageBuffer;
}

// -------------------------------------------------------------------------------------------------------------------------
void Image::copyBuffer(VkBuffer buffer) const
{
  auto image = *image_;
  auto width = static_cast<uint32_t>(asset_->width());
  auto height = static_cast<uint32_t>(asset_->height());
  device_->submitCommand([buffer, image, width, height](auto cmd) {
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };
    vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  });
}

// -------------------------------------------------------------------------------------------------------------------------
void Image::generateMipMaps()
{
  VkFormatProperties formatProperties;
  vkGetPhysicalDeviceFormatProperties(device_->physical(), format_, &formatProperties);

  if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
    throw std::runtime_error("texture image format does not support linear blitting!");
  }

  std::cout << mipLevels_ << std::endl;

  device_->submitCommand([mipLevels = mipLevels_, image = *image_, width = asset_->width(), height = asset_->height()](auto cmd) {
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = width;
    int32_t mipHeight = height;

    for (uint32_t i = 1; i < mipLevels; i++) {
      barrier.subresourceRange.baseMipLevel = i - 1;
      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

      vkCmdPipelineBarrier(cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

      VkImageBlit blit = {};
      blit.srcOffsets[0] = { 0, 0, 0 };
      blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
      blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.srcSubresource.mipLevel = i - 1;
      blit.srcSubresource.baseArrayLayer = 0;
      blit.srcSubresource.layerCount = 1;
      blit.dstOffsets[0] = { 0, 0, 0 };
      blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
      blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.dstSubresource.mipLevel = i;
      blit.dstSubresource.baseArrayLayer = 0;
      blit.dstSubresource.layerCount = 1;

      vkCmdBlitImage(cmd,
        image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &blit,
        VK_FILTER_LINEAR);

      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      vkCmdPipelineBarrier(cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

      if (mipWidth > 1) mipWidth /= 2;
      if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(cmd,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0,
      0, nullptr,
      0, nullptr,
      1, &barrier);
  });
}

// -------------------------------------------------------------------------------------------------------------------------
void Image::changeLayout(VkImageLayout oldLayout, VkImageLayout newLayout) const
{
  device_->submitCommand([oldLayout, newLayout, image = *image_, mipLevels = this->mipLevels_](auto cmd) {
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
      THROW("Unsupported image layout transition");
    }
    vkCmdPipelineBarrier(
      cmd,
      sourceStage,
      destinationStage,
      0,
      0, nullptr,
      0, nullptr,
      1, &barrier
    );
  });
}

} // !namespace yaga
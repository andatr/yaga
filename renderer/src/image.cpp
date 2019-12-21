#include "precompiled.h"
#include "image.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Image::Image(Device* device, Allocator* allocator, VkExtent2D size, VkFormat format, VkImageUsageFlags usage)
{
  auto ldevice = device->Logical();
  CreateImage(ldevice, allocator, size, format, usage);
}

// -------------------------------------------------------------------------------------------------------------------------
Image::Image(Device* device, Allocator* allocator, asset::Texture* asset)
{
  auto ldevice = device->Logical();
  CreateImage(ldevice, allocator, { static_cast<uint32_t>(asset->Width()), static_cast<uint32_t>(asset->Height()) },
    VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
  auto buffer = CreateBuffer(ldevice, allocator, asset);
  ChangeLayout(device, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  CopyBuffer(device, **buffer, asset);
  ChangeLayout(device, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

// -------------------------------------------------------------------------------------------------------------------------
void Image::CreateImage(VkDevice device, Allocator* allocator, VkExtent2D size, VkFormat format, VkImageUsageFlags usage)
{
  VkImageCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.extent.width = size.width;
  info.extent.height = size.height;
  info.extent.depth = 1;
  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.format = format;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage = usage;
  info.samples = VK_SAMPLE_COUNT_1_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  auto destroyImage = [device](auto image) {
    vkDestroyImage(device, image, nullptr);
    LOG(trace) << "Image destroyed";
  };
  VkImage image;
  if (vkCreateImage(device, &info, nullptr, &image) != VK_SUCCESS) {
    THROW("Could not create Image");
  }
  image_.Assign(image, destroyImage);
  memory_ = allocator->AllocateImage(image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkBindImageMemory(device, image, *memory_, 0);
}

// -------------------------------------------------------------------------------------------------------------------------
DeviceBufferPtr Image::CreateBuffer(VkDevice device, Allocator* allocator, asset::Texture* asset) const
{
  auto stageBuffer = std::make_unique<DeviceBuffer>(device, allocator, asset->Size(),
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stageBuffer->Update(asset->Data(), asset->Size());
  return stageBuffer;
}

// -------------------------------------------------------------------------------------------------------------------------
void Image::CopyBuffer(Device* device, VkBuffer buffer, asset::Texture* asset) const
{
  auto image = *image_;
  auto width = static_cast<uint32_t>(asset->Width());
  auto height = static_cast<uint32_t>(asset->Height());
  device->SubmitCommand([buffer, image, width, height](auto cmd) {
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
void Image::ChangeLayout(Device* device, VkImageLayout oldLayout, VkImageLayout newLayout) const
{
  auto image = *image_;
  device->SubmitCommand([oldLayout, newLayout, image](auto cmd) {
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
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
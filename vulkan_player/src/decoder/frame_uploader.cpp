#include "precompiled.h"
#include "frame_uploader.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
void changeLayoutUndefinedTransferDst(VkCommandBuffer command, VkImage image)
{
  VkImageMemoryBarrier barrier{};
  barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image                           = image;
  barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.srcAccessMask                   = VK_ACCESS_NONE_KHR;
  barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel   = 0;
  barrier.subresourceRange.levelCount     = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount     = 1;
  vkCmdPipelineBarrier(
    command,
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
  );
}

// -----------------------------------------------------------------------------------------------------------------------------
void changeLayoutTransferDstGeneral(VkCommandBuffer command, VkImage image)
{
  VkImageMemoryBarrier barrier{};
  barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image                           = image;
  barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout                       = VK_IMAGE_LAYOUT_GENERAL;
  barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;
  barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel   = 0;
  barrier.subresourceRange.levelCount     = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount     = 1;
  vkCmdPipelineBarrier(
    command,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
  );
}

// -----------------------------------------------------------------------------------------------------------------------------
void setImageData(VkCommandBuffer command, VkBuffer buffer, VkDeviceSize offset, Image* image)
{
  VkBufferImageCopy region{};
  region.bufferOffset                    = offset;
  region.bufferRowLength                 = image->info().extent.width;
  region.bufferImageHeight               = image->info().extent.height;
  region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel       = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount     = 1;
  region.imageOffset                     = { 0, 0, 0 };
  region.imageExtent                     = { 
    image->info().extent.width,
    image->info().extent.height,
    1
  };
  vkCmdCopyBufferToImage(command, buffer, **image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

// -----------------------------------------------------------------------------------------------------------------------------
VkFormat getLumaFormat(uint8_t bytes)
{
  if (bytes == 1) return VK_FORMAT_R8_UINT;
  if (bytes == 2) return VK_FORMAT_R16_UINT;
  if (bytes == 4) return VK_FORMAT_R32_UINT;
  THROW("Unknown luma bpp %1%", bytes);
}

// -----------------------------------------------------------------------------------------------------------------------------
VkFormat getChromaFormat(uint8_t bytes)
{
  if (bytes == 2) return VK_FORMAT_R8G8_UINT;
  if (bytes == 4) return VK_FORMAT_R16G16_UINT;
  if (bytes == 8) return VK_FORMAT_R32G32_UINT;
  THROW("Unknown chroma bpp %1%", bytes);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
FrameUploader::FrameUploader(Swapchain* swapchain, VmaAllocator allocator) :
  swapchain_(swapchain),
  allocator_(allocator)
{
  createCommand();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameUploader::changeFormat(FormatPtr format)
{
  vkWaitForFences(**swapchain_->device(), 1, &*fence_, VK_TRUE, UINT64_MAX);
  format_ = format;
  createLumaImage();
  createChromaImage();
  createBuffer();
}

// -----------------------------------------------------------------------------------------------------------------------------
bool FrameUploader::upload(Frame* frame)
{
  if (frame->format() != format_) return false;

  vkWaitForFences(**swapchain_->device(), 1, &*fence_, VK_TRUE, UINT64_MAX);
  vkResetFences  (**swapchain_->device(), 1, &*fence_);

  const auto offset = static_cast<VkDeviceSize>(frame->format()->luma.size);
  memcpy(buffer_->memory().pMappedData, frame->data(), frame->format()->frameSize);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VULKAN_GUARD(vkBeginCommandBuffer(*command_, &beginInfo), "Could not begin Copy Command");
  changeLayoutUndefinedTransferDst (*command_, **lumaImage_);
  changeLayoutUndefinedTransferDst (*command_, **chromaImage_);
  setImageData                     (*command_, **buffer_, 0, lumaImage_.get());
  setImageData                     (*command_, **buffer_, offset, chromaImage_.get());
  changeLayoutTransferDstGeneral   (*command_, **lumaImage_);
  changeLayoutTransferDstGeneral   (*command_, **chromaImage_);
  VULKAN_GUARD(vkEndCommandBuffer  (*command_), "Could not finish Copy Command");

  VkSubmitInfo submitInfo{};
  submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &*command_;

  VULKAN_GUARD(vkQueueSubmit(swapchain_->device()->graphicsQueue(), 1, &submitInfo, *fence_), "Could not submit Copy Command");
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameUploader::createBuffer()
{
  if (buffer_ && buffer_->memory().size >= format_->frameSize) return;

  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size  = format_->frameSize;
  info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
  allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

  buffer_ = std::make_unique<Buffer>(allocator_, info, allocInfo);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameUploader::createLumaImage()
{
  VkImageCreateInfo info{};
  info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType     = VK_IMAGE_TYPE_2D;
  info.extent.width  = static_cast<uint32_t>(format_->luma.width);
  info.extent.height = static_cast<uint32_t>(format_->luma.height);
  info.extent.depth  = 1;
  info.mipLevels     = 1;
  info.arrayLayers   = 1;
  info.format        = getLumaFormat(format_->luma.bytes);
  info.tiling        = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  info.samples       = VK_SAMPLE_COUNT_1_BIT;
  info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format   = info.format;
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
  viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
  viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel   = 0;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount     = 1;
  viewInfo.subresourceRange.levelCount     = info.mipLevels;

  lumaImage_ = std::make_unique<Image>(swapchain_->device(), allocator_, info, viewInfo);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameUploader::createChromaImage()
{
  VkImageCreateInfo info{};
  info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType     = VK_IMAGE_TYPE_2D;
  info.extent.width  = static_cast<uint32_t>(format_->chroma.width);
  info.extent.height = static_cast<uint32_t>(format_->chroma.height);
  info.extent.depth  = 1;
  info.mipLevels     = 1;
  info.arrayLayers   = 1;
  info.format        = getChromaFormat(format_->chroma.bytes);
  info.tiling        = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  info.samples       = VK_SAMPLE_COUNT_1_BIT;
  info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format   = info.format;
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
  viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel   = 0;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount     = 1;
  viewInfo.subresourceRange.levelCount     = info.mipLevels;

  chromaImage_ = std::make_unique<Image>(swapchain_->device(), allocator_, info, viewInfo);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameUploader::createCommand()
{
  const auto device = **swapchain_->device();
  const auto pool   = swapchain_->device()->commandPool();

  VkCommandBufferAllocateInfo info{};
  info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  info.commandPool        = pool;
  info.commandBufferCount = 1;

  command_ = vk::createCommand(device, pool, info);
  fence_   = vk::createFence  (device);
}

} // !namespace vk
} // !namespace yaga
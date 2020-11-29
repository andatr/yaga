#include "precompiled.h"
#include "image.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Image::Image(Device* device, VmaAllocator allocator, const VkImageCreateInfo& info, VkImageViewCreateInfo viewInfo,
  const VmaAllocationCreateInfo& allocInfo, const VkSampler sampler) :
  vkDevice_(**device),
  allocator_(allocator), image_(VK_NULL_HANDLE), allocation_{}, memory_{}, imageView_(VK_NULL_HANDLE), sampler_(sampler),
  info_(info)
{
  VULKAN_GUARD(vmaCreateImage(allocator_, &info, &allocInfo, &image_, &allocation_, nullptr),
    "Could not create Vulkan Image");
  LOG(trace) << "Vulkan Image created";
  viewInfo.image = image_;
  viewInfo.format = info.format;
  viewInfo.subresourceRange.levelCount = info.mipLevels;
  if (vkCreateImageView(vkDevice_, &viewInfo, nullptr, &imageView_) != VK_SUCCESS) {
    vmaDestroyImage(allocator_, image_, allocation_);
    image_ = VK_NULL_HANDLE;
    LOG(trace) << "Vulkan Image destroyed";
    THROW("Could not create Vulkan Image View");
  }
  LOG(trace) << "Vulkan Image View created";
}

// -----------------------------------------------------------------------------------------------------------------------------
Image::~Image()
{
  vkDestroyImageView(vkDevice_, imageView_, nullptr);
  LOG(trace) << "Vulkan Image View destroyed";
  vmaDestroyImage(allocator_, image_, allocation_);
  LOG(trace) << "Vulkan Image destroyed";
}

} // !namespace vk
} // !namespace yaga
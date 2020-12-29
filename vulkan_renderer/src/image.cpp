#include "precompiled.h"
#include "image.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Image::Image(Device* device, VmaAllocator allocator, VkImageCreateInfo& info, VkImageViewCreateInfo& viewInfo) :
  allocator_(allocator),
  info_(info)
{
  auto destroyImage = [this](auto image) {
    vmaDestroyImage(allocator_, image, allocation_);
    LOG(trace) << "Vulkan Image destroyed";
  };

  auto destroyView = [device = **device](auto view) {
    vkDestroyImageView(device, view, nullptr);
    LOG(trace) << "Image View destroyed";
  };

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkImage image;
  VULKAN_GUARD(vmaCreateImage(allocator_, &info, &allocInfo, &image, &allocation_, nullptr), "Could not create Image");
  LOG(trace) << "Vulkan Image created";
  image_.set(image, destroyImage);
  
  VkImageView view;
  viewInfo.image = *image_;
  VULKAN_GUARD(vkCreateImageView(**device, &viewInfo, nullptr, &view), "Could not create Image View");
  LOG(trace) << "Image View created";
  view_.set(view, destroyView);
}

// -----------------------------------------------------------------------------------------------------------------------------
Image::~Image()
{
}

} // !namespace vk
} // !namespace yaga
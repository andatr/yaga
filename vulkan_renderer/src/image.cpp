#include "precompiled.h"
#include "vulkan_renderer/image.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Image::Image(Device* device, VmaAllocator allocator, VkImageCreateInfo& info, VkImageViewCreateInfo& viewInfo) :
  device_(**device),
  allocator_(allocator),
  info_(info)
{
  creatImage(info);
  creatImageView(viewInfo);
}

// -----------------------------------------------------------------------------------------------------------------------------
Image::~Image()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Image::creatImage(VkImageCreateInfo& info)
{
  info_ = info;
  image_ = vk::createImage(allocator_, info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Image::creatImageView(VkImageViewCreateInfo& info)
{
  info.image = *image_;
  view_ = vk::createImageView(device_, info);
}

} // !namespace vk
} // !namespace yaga
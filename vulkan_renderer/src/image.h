#ifndef YAGA_VULKAN_RENDERER_SRC_IMAGE
#define YAGA_VULKAN_RENDERER_SRC_IMAGE

#include <memory>
#include <boost/noncopyable.hpp>

#include "device.h"
#include "vulkan.h"

namespace yaga {
namespace vk {

class Image : private boost::noncopyable
{
public:
  Image(Device* device, VmaAllocator allocator, const VkImageCreateInfo& info, VkImageViewCreateInfo viewInfo,
    const VmaAllocationCreateInfo& allocInfo, VkSampler sampler = VK_NULL_HANDLE);
  ~Image();
  const VkImageCreateInfo& info() const { return info_; }
  VkSampler sampler() const { return sampler_; }
  VkImage operator*() const { return image_; }
  VkImageView view() const { return imageView_; }
  const VmaAllocationInfo& memory() const { return memory_; }

private:
  VkDevice vkDevice_;
  VmaAllocator allocator_;
  VkImage image_;
  VmaAllocation allocation_;
  VmaAllocationInfo memory_;
  VkImageView imageView_;
  VkSampler sampler_;
  VkImageCreateInfo info_;
};

typedef std::unique_ptr<Image> ImagePtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_IMAGE

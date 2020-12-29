#ifndef YAGA_VULKAN_RENDERER_SRC_IMAGE
#define YAGA_VULKAN_RENDERER_SRC_IMAGE

#include <memory>
#include <boost/noncopyable.hpp>

#include "device.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Image : private boost::noncopyable
{
public:
  explicit Image(Device* device, VmaAllocator allocator, VkImageCreateInfo& info, VkImageViewCreateInfo& viewInfo);
  virtual ~Image();
  VkImage operator*() const { return *image_; }
  VkImageView  view() const { return *view_;  }
  const VkImageCreateInfo& info() const { return info_; }

private:
  VmaAllocator allocator_;
  VkImageCreateInfo info_;
  AutoDestructor<VkImage> image_;
  AutoDestructor<VkImageView> view_;
  VmaAllocation allocation_;
};

typedef std::unique_ptr<Image> ImagePtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_IMAGE

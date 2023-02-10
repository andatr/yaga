#ifndef YAGA_VULKAN_RENDERER_IMAGE
#define YAGA_VULKAN_RENDERER_IMAGE

#include "utility/compiler.h"

#include <memory>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "vulkan_renderer/device.h"
#include "vulkan_renderer/vulkan.h"
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
  void creatImage(VkImageCreateInfo& info);
  void creatImageView(VkImageViewCreateInfo& info);

private:
  VkDevice device_;
  VmaAllocator allocator_;
  VkImageCreateInfo info_;
  AutoDestructor<VkImage> image_;
  AutoDestructor<VkImageView> view_;
};

typedef std::unique_ptr<Image> ImagePtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_IMAGE

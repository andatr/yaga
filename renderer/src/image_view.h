#ifndef YAGA_RENDERER_SRC_IMAGE_VIEW
#define YAGA_RENDERER_SRC_IMAGE_VIEW

#include <memory>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "utility/auto_destroyer.h"

namespace yaga
{

class ImageView : private boost::noncopyable
{
public:
  ImageView(VkDevice device, VkImage image, VkFormat format);
  VkImageView operator*() const { return *imageView_; }
private:
  AutoDestroyer<VkImageView> imageView_;
};

typedef std::unique_ptr<ImageView> ImageViewPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_IMAGE_VIEW

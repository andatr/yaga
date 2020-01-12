#ifndef YAGA_RENDERER_SRC_IMAGE_VIEW
#define YAGA_RENDERER_SRC_IMAGE_VIEW

#include <memory>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "image.h"
#include "utility/auto_destructor.h"

namespace yaga
{

class ImageView : private boost::noncopyable
{
public:
  ImageView(Device* device, Image* image, VkImageAspectFlagBits aspectMask);
  VkImageView operator*() const { return *imageView_; }
  Image* image() const { return image_; }
private:
  void createImageView();
private:
  Device* device_;
  VkDevice vkDevice_;
  Image* image_;
  VkImageAspectFlagBits aspectMask_;
  AutoDestructor<VkImageView> imageView_;
};

typedef std::unique_ptr<ImageView> ImageViewPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_IMAGE_VIEW

#ifndef YAGA_RENDERER_SRC_TEXTURE
#define YAGA_RENDERER_SRC_TEXTURE

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "utility/auto_destroyer.h"

namespace yaga
{

class Texture : private boost::noncopyable
{
public:
  Texture(VkDevice device, VkImage image, VkFormat format);
  VkImageView ImageView() const { return *imageView_; }
private:
  AutoDestroyer<VkImageView> imageView_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_TEXTURE

#ifndef YAGA_RENDERER_SRC_VIDEO_BUFFER
#define YAGA_RENDERER_SRC_VIDEO_BUFFER

#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "texture.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class VideoBuffer : private boost::noncopyable
{
public:
  VideoBuffer(Device* device, VkSurfaceKHR surface, VkExtent2D size);
  const VkFormat& ImageFormat() const { return format_; }
  const VkExtent2D& Size() const { return size_; }
  const std::vector<std::unique_ptr<Texture>>& Textures() const { return textures_; }
  VkSwapchainKHR Swapchain() const { return *swapchain_; }
  virtual ~VideoBuffer();
private:
  VkFormat format_;
  VkExtent2D size_;
  AutoDestroyer<VkSwapchainKHR> swapchain_;
  std::vector<VkImage> images_;
  std::vector<std::unique_ptr<Texture>> textures_;
};

typedef std::unique_ptr<VideoBuffer> VideoBufferPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_VIDEO_BUFFER

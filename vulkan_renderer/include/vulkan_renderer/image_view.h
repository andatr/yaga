#ifndef YAGA_VULKAN_RENDERER_IMAGE_VIEW
#define YAGA_VULKAN_RENDERER_IMAGE_VIEW

#include "vulkan_renderer/image.h"
#include "vulkan_renderer/vulkan.h"

namespace yaga {
namespace vk {

struct ImageView
{
  VkImage data;
  VkImageView view;
  
  ImageView() : data{}, view{} {}

  ImageView(VkImage d, VkImageView v) : data(d), view(v) {}

  ImageView(Image& image) :
    data(*image),
    view(image.view()) {}
  
  ImageView(Image* image) :
    data(**image),
    view(image->view()) {}

  ImageView(ImagePtr& image) :
    data(**image),
    view(image->view()) {}
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_IMAGE_VIEW

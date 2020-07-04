#ifndef YAGA_VULKAN_RENDERER_SRC_PRESENTER
#define YAGA_VULKAN_RENDERER_SRC_PRESENTER

#include <memory>
#include <vector>

#include "device.h"
#include "swapchain.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class Presenter
{
public:
  explicit Presenter(Device* device, Swapchain* swapchain);
  void waitPrevFrame();
  bool acquireImage(uint32_t* image);
  bool present(VkCommandBuffer command, uint32_t image);
  void swapchain(Swapchain* swapchain);
private:
  struct FrameSync
  {
    AutoDestructor<VkSemaphore> render;
    AutoDestructor<VkSemaphore> present;
    AutoDestructor<VkFence> swap;
  };
private:
  void createSync();
private:
  Device* device_;
  VkDevice vkDevice_;
  VkSwapchainKHR swapchain_;
  std::vector<FrameSync> frameSync_;
  size_t frame_;
};

typedef std::unique_ptr<Presenter> PresenterPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_PRESENTER
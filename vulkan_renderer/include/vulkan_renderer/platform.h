#ifndef YAGA_VULKAN_RENDERER_PLATFORM
#define YAGA_VULKAN_RENDERER_PLATFORM

#include <memory>
#include <string>
#include <vector>

#include "vulkan_renderer/context.h"
#include "vulkan_renderer/device.h"
#include "vulkan_renderer/renderer.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"
#include "vulkan_renderer/vulkan_utils.h"
#include "vulkan_renderer/window.h"
#include "engine/platform.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Platform : public yaga::Platform
{
public:
  explicit Platform();
  virtual ~Platform();
  void run(ApplicationPtr app) override;

private:
  void loop();

private:
  bool running_;
  ApplicationPtr app_;
  AutoDestructor<VkInstance> instance_;
  WindowPtr window_;  
  AutoDestructor<VkDebugUtilsMessengerEXT> debugMessenger_;
  DevicePtr device_;
  AutoDestructor<VmaAllocator> allocator_;
  SwapchainPtr swapchain_;
  RendererPtr renderer_;
  ContextPtr context_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_PLATFORM

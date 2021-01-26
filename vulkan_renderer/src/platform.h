#ifndef YAGA_VULKAN_RENDERER_SRC_PLATFORM
#define YAGA_VULKAN_RENDERER_SRC_PLATFORM

#include <memory>
#include <string>
#include <vector>

#include "config.h"
#include "context.h"
#include "device.h"
#include "renderer.h"
#include "swapchain.h"
#include "vulkan.h"
#include "window.h"
#include "engine/platform.h"
#include "utility/auto_destructor.h"
#include "vulkan_renderer/platform.h"

namespace yaga {
namespace vk {

class Platform : public yaga::Platform
{
public:
  class InitGLFW
  {
  public:
    InitGLFW();
    ~InitGLFW();
  };

public:
  explicit Platform(const Config& config);
  virtual ~Platform();
  void run(Application* app) override;

private:
  VulkanExtensions createInstance(const std::string& appName);
  void createAllocator();
  void checkValidationLayers() const;
  void setupLogging();
  void loop();

private:
  static InitGLFW initGLFW_;
  const Config config_;
  bool running_;
  AutoDestructor<VkInstance> instance_;
  WindowPtr window_;  
  AutoDestructor<VkDebugUtilsMessengerEXT> debugMessenger_;
  DevicePtr device_;
  AutoDestructor<VmaAllocator> allocator_;
  SwapchainPtr swapchain_;
  ContextPtr context_;
  RendererPtr renderer_;
  Application* app_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_PLATFORM

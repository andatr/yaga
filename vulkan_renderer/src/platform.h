#ifndef YAGA_VULKAN_RENDERER_SRC_PLATFORM
#define YAGA_VULKAN_RENDERER_SRC_PLATFORM

#include <memory>
#include <string>
#include <vector>
#include <boost/dll/alias.hpp>

#include "context.h"
#include "device.h"
#include "renderer.h"
#include "swapchain.h"
#include "vulkan.h"
#include "window.h"
#include "assets/application.h"
#include "engine/platform.h"
#include "utility/auto_destructor.h"

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
  explicit Platform(const assets::Application* asset);
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
  const assets::Application* asset_;
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

PlatformPtr createPlatform(const assets::Application* asset);

} // !namespace vk
} // !namespace yaga

BOOST_DLL_ALIAS(yaga::vk::createPlatform, createPlatform)

#endif // !YAGA_VULKAN_RENDERER_SRC_PLATFORM

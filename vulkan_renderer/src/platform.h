#ifndef YAGA_VULKAN_RENDERER_SRC_PLATFORM
#define YAGA_VULKAN_RENDERER_SRC_PLATFORM

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <boost/dll/alias.hpp>

#include "device.h"
#include "event_dispatcher.h"
#include "input.h"
#include "presenter.h"
#include "renderer.h"
#include "rendering_context.h"
#include "swapchain.h"
#include "vulkan.h"
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
  RenderingContext* renderingContext() override { return renderingContext_.get(); }
  Input* input() override { return input_.get(); }

private:
  void createWindow();
  VulkanExtensions createInstance(const std::string& appName);
  void createSurface();
  void createAllocator();
  void resize(Application* app);
  void checkValidationLayers() const;
  void setupLogging();
  void setupGUI();
  void loop(Application* app);
  void gameLoop(Application* app);
  void drawFrame(Application* app);
  VkExtent2D getWindowSize() const;
  void onResize(int width, int height);

private:
  static InitGLFW initGLFW_;
  const assets::Application* asset_;
  AutoDestructor<GLFWwindow*> window_;
  EventDispatcherPtr eventDispatcher_;
  InputPtr input_;
  AutoDestructor<VkInstance> instance_;
  AutoDestructor<VkDebugUtilsMessengerEXT> debugMessenger_;
  AutoDestructor<VkSurfaceKHR> surface_;
  DevicePtr device_;
  AutoDestructor<VmaAllocator> allocator_;
  SwapchainPtr swapchain_;
  RenderingContextPtr renderingContext_;
  RendererPtr renderer_;
  PresenterPtr presenter_;
  std::atomic_bool minimised_;
  std::atomic_bool resized_;
  std::chrono::high_resolution_clock::time_point startTime_;
  EventDispatcher::Connection resizeConnection_;
};

PlatformPtr createPlatform(const assets::Application* asset);

} // !namespace vk
} // !namespace yaga

BOOST_DLL_ALIAS(yaga::vk::createPlatform, createPlatform)

#endif // !YAGA_VULKAN_RENDERER_SRC_PLATFORM

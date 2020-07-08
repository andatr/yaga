#ifndef YAGA_VULKAN_RENDERER_SRC_APPLICATION
#define YAGA_VULKAN_RENDERER_SRC_APPLICATION

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <boost/dll/alias.hpp>

#include "device.h"
#include "presenter.h"
#include "renderer.h"
#include "rendering_context.h"
#include "swapchain.h"
#include "vulkan.h"
#include "assets/application.h"
#include "engine/application.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Application : public yaga::Application
{
public:
  class InitGLFW
  {
  public:
    InitGLFW();
    ~InitGLFW();
  };

public:
  explicit Application(GamePtr game, const assets::Application* asset);
  virtual ~Application();
  void run() override;
  RenderingContext* renderingContext() override { return renderingContext_.get(); }

private:
  static void resizeCallback(GLFWwindow* window, int width, int height);
  void createWindow();
  VulkanExtensions createInstance(const std::string& appName);
  void createSurface();
  void createAllocator();
  void resize();
  void checkValidationLayers() const;
  void setupLogging();
  void loop();
  void gameLoop();
  void drawFrame();
  VkExtent2D getWindowSize() const;

private:
  static InitGLFW initGLFW_;
  const assets::Application* asset_;
  AutoDestructor<GLFWwindow*> window_;
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
};

ApplicationPtr createApplication(GamePtr game, const assets::Application* asset);
BOOST_DLL_ALIAS(yaga::vk::createApplication, createApplication)

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_APPLICATION_IMPL

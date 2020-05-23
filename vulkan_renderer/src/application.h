#ifndef YAGA_VULKAN_RENDERER_SRC_APPLICATION
#define YAGA_VULKAN_RENDERER_SRC_APPLICATION

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "device.h"
#include "presenter.h"
#include "renderer.h"
#include "rendering_context.h"
#include "swapchain.h"
#include "vulkan.h"
#include "engine/application.h"
#include "engine/asset/application.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

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
  explicit Application(Game* game, const asset::Application* asset);
  virtual ~Application();
  void run() override;
  RenderingContext* renderingContext() override { return renderingContext_.get(); }
private:
  void createWindow();
  VulkanExtensions createInstance(const std::string& appName);
  void createSurface();
  void createAllocator();
  void resize();
  void checkValidationLayers() const;
  void setupLogging();
  void loop();
  void drawFrame();
  VkExtent2D getWindowSize() const;
  static void resizeCallback(GLFWwindow* window, int width, int height);
private:
  struct ResizeInfo
  {
    std::atomic_bool resized;
    VkExtent2D size;
    ResizeInfo() : resized(false), size{} {}
  };
private:
  static InitGLFW initGLFW_;
  const asset::Application* asset_;
  ResizeInfo resize_;
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
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_APPLICATION_IMPL

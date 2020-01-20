#ifndef YAGA_RENDERER_SRC_APPLICATION_IMPL
#define YAGA_RENDERER_SRC_APPLICATION_IMPL

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device.h"
#include "image.h"
#include "image_view.h"
#include "render_object.h"
#include "swapchain.h"
#include "engine/application.h"
#include "engine/asset/application.h"
#include "utility/array.h"
#include "utility/auto_destructor.h"

namespace yaga
{

class VulkanApp : public Application
{
public:
  class InitGLFW
  {
  public:
    InitGLFW();
    ~InitGLFW();
  };
public:
  explicit VulkanApp();
  virtual ~VulkanApp();
  void run() override;
private:
  void createWindow(asset::Application* props);
  void createInstance(const std::string& appName);
  void createSwapchain(VkExtent2D size);
  void createSurface();
  void checkValidationLayers();
  void loop();
  void setupLogging();
  void createSync();
  void drawFrame();
  VkExtent2D getWindowSize() const;
  static void resizeCallback(GLFWwindow* window, int width, int height);
private:
  struct FrameSync
  {
    AutoDestructor<VkSemaphore> render;
    AutoDestructor<VkSemaphore> present;
    AutoDestructor<VkFence> swap;
  };
  struct ResizeInfo
  {
    std::atomic_bool resized;
    VkExtent2D size;
    ResizeInfo() : resized(false), size{} {}
  };
private:
  static InitGLFW initGLFW_;
  size_t frame_;
  ResizeInfo resize_;
  AutoDestructor<GLFWwindow*> window_;
  AutoDestructor<VkInstance> instance_;
  AutoDestructor<VkDebugUtilsMessengerEXT> debugMessenger_;
  AutoDestructor<VkSurfaceKHR> surface_;
  DevicePtr device_;
  AllocatorPtr allocator_;
  SwapchainPtr swapchain_;
  RenderObjectPtr model_;
  MeshPtr mesh_;
  MaterialPtr material_;
  ImagePtr image_;
  ImageViewPtr imageView_;
  std::vector<FrameSync> frameSync_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_APPLICATION_IMPL
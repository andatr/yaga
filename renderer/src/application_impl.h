#ifndef YAGA_RENDERER_SRC_APPLICATION_IMPL
#define YAGA_RENDERER_SRC_APPLICATION_IMPL

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <GLFW/glfw3.h>

#include "allocator.h"
#include "application.h"
#include "device.h"
#include "image.h"
#include "image_view.h"
#include "model.h"
#include "video_buffer.h"
#include "asset/application.h"
#include "asset/database.h"
#include "utility/array.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class ApplicationImpl : public Application
{
public:
  class InitGLFW
  {
  public:
    InitGLFW();
    ~InitGLFW();
  };
public:
  explicit ApplicationImpl();
  virtual ~ApplicationImpl();
  void Run(const std::string& dir) override;
private:
  void CreateWindow(asset::Application* props);
  void CreateInstance(const std::string& appName);
  void CreateVideoBuffer(VkExtent2D size);
  void CreateSurface();
  
  void CheckValidationLayers();
  void Loop();
  void SetupLogging();
  void CreateSync();
  void DrawFrame();
  VkExtent2D GetWindowSize() const;
  static void ResizeCallback(GLFWwindow* window, int width, int height);
private:
  struct FrameSync
  {
    AutoDestroyer<VkSemaphore> render;
    AutoDestroyer<VkSemaphore> present;
    AutoDestroyer<VkFence> swap;
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
  asset::DatabasePtr assets_;
  AutoDestroyer<GLFWwindow*> window_;
  AutoDestroyer<VkInstance> instance_;
  AutoDestroyer<VkDebugUtilsMessengerEXT> debugMessenger_;
  AutoDestroyer<VkSurfaceKHR> surface_;
  DevicePtr device_;
  AllocatorPtr allocator_;
  VideoBufferPtr videoBuffer_;
  ModelPtr model_;
  MeshPtr mesh_;
  MaterialPtr material_;
  ImagePtr image_;
  ImageViewPtr imageView_;
  std::vector<FrameSync> frameSync_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_APPLICATION_IMPL
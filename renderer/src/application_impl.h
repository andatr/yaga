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
  // don't reorder - destruction order is important
  std::unique_ptr<asset::Database> assets_;
  AutoDestroyer<GLFWwindow*> window_;
  AutoDestroyer<VkInstance> instance_;
  AutoDestroyer<VkDebugUtilsMessengerEXT> debugMessenger_;
  AutoDestroyer<VkSurfaceKHR> surface_;
  std::unique_ptr<Device> device_;
  std::unique_ptr<Allocator> allocator_;
  std::unique_ptr<VideoBuffer> videoBuffer_;
  std::unique_ptr<Model> model_;
  std::unique_ptr<Mesh> mesh_;
  std::unique_ptr<Material> material_;
  std::vector<FrameSync> frameSync_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_APPLICATION_IMPL
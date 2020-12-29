#ifndef YAGA_VULKAN_RENDERER_SRC_WINDOW
#define YAGA_VULKAN_RENDERER_SRC_WINDOW

#include <memory>
#include <thread>

#include "event_dispatcher.h"
#include "input.h"
#include "vulkan.h"
#include "assets/application.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Window
{
public:
  Window(VkInstance instance, const assets::Application* asset);
  ~Window();
  VkInstance   instance() const { return instance_;    }
  Input*          input() const { return input_.get(); }
  GLFWwindow* operator*() const { return *window_;     }
  VkSurfaceKHR  surface() const { return *surface_;    }
  bool          resized() const { return resized_;     }
  bool        minimised() const { return minimised_;   }
  VkExtent2D       size() const { return size_;        }
  void resized(bool value) { resized_ = value; }
  void update();

private:
  void createWindow(const assets::Application* asset);
  void createSurface(VkInstance instance);
  void updateSize();

private:
  VkInstance instance_;
  VkExtent2D size_;
  bool minimised_;
  bool resized_;
  AutoDestructor<GLFWwindow*> window_;
  AutoDestructor<VkSurfaceKHR> surface_;
  EventDispatcherPtr eventDispatcher_;
  InputPtr input_;
};

typedef std::unique_ptr<Window> WindowPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_WINDOW

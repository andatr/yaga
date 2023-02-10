#ifndef YAGA_VULKAN_RENDERER_WINDOW
#define YAGA_VULKAN_RENDERER_WINDOW

#include <memory>
#include <thread>

#include "vulkan_renderer/event_dispatcher.h"
#include "vulkan_renderer/input.h"
#include "vulkan_renderer/vulkan.h"
#include "engine/config.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Window
{
public:
  Window(VkInstance instance, const Config::Window& config);
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
  void createWindow(const Config::Window& config);
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
  VkInputPtr input_;
};

typedef std::unique_ptr<Window> WindowPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_WINDOW

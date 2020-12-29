#include "precompiled.h"
#include "window.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Window::Window(VkInstance instance, const assets::Application* asset) :
  instance_(instance),
  size_{},
  minimised_(false),
  resized_(false)
{
  namespace ph = std::placeholders;

  createWindow(asset);
  createSurface(instance);
  updateSize();
  eventDispatcher_ = std::make_unique<EventDispatcher>(*window_);
  input_ = std::make_unique<Input>(*window_, eventDispatcher_.get());
}

// -----------------------------------------------------------------------------------------------------------------------------
Window::~Window()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Window::createWindow(const assets::Application* asset)
{
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  auto deleteWindow = [](auto window) {
    glfwDestroyWindow(window);
    LOG(trace) << "Window destroyed";
  };
  auto window = glfwCreateWindow(asset->width(), asset->height(), asset->title().c_str(), nullptr, nullptr);
  if (!window) {
    THROW("Could not create Window");
  }
  window_.set(window, deleteWindow);
  LOG(trace) << "Window created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Window::createSurface(VkInstance instance)
{
  auto destroySurface = [instance](auto surface) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
    LOG(trace) << "Surface destroyed";
  };
  VkSurfaceKHR surface;
  VULKAN_GUARD(glfwCreateWindowSurface(instance, *window_, nullptr, &surface), "Could not create Window Surface");
  surface_.set(surface, destroySurface);
  LOG(trace) << "Surface created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Window::updateSize()
{
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(*window_, &width, &height);
  const VkExtent2D newSize = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
  minimised_ == (width == 0) && (height == 0);
  resized_   == (size_.width != newSize.width) || (size_.height != newSize.height);
  size_ = newSize;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Window::update()
{
  glfwPollEvents();
  updateSize();
  input_->updateState();
}

} // !namespace vk
} // !namespace yaga
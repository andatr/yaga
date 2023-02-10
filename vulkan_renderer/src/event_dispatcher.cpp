#include "precompiled.h"
#include "vulkan_renderer/event_dispatcher.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
EventDispatcher::EventDispatcher(GLFWwindow* window) : window_(window)
{
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetKeyCallback            (window, keyboardCallback);
  glfwSetCharCallback           (window, characterCallback);
  glfwSetMouseButtonCallback    (window, mouseButtonCallback);
  glfwSetCursorPosCallback      (window, cursorMoveCallback);
  glfwSetCursorEnterCallback    (window, cursorLeaveCallback);
}

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnectionPtr EventDispatcher::onResize(const SignalResize::slot_type& handler)
{
  return std::make_unique<SignalConnection>(sigResize_.connect(handler));
}

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnectionPtr EventDispatcher::onKeyboard(const SignalKeyboard::slot_type&    handler)
{
  return std::make_unique<SignalConnection>(sigKeyboard_.connect(handler));
}

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnectionPtr EventDispatcher::onCharacter(const SignalCharacter::slot_type&   handler)
{
  return std::make_unique<SignalConnection>(sigCharacter_.connect(handler));
}

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnectionPtr EventDispatcher::onMouseButton(const SignalMouseButton::slot_type& handler)
{
  return std::make_unique<SignalConnection>(sigMouseButton_.connect(handler));
}

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnectionPtr EventDispatcher::onCursorMove(const SignalCursorMove::slot_type&  handler)
{
  return std::make_unique<SignalConnection>(sigCursorMove_.connect(handler));
}

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnectionPtr EventDispatcher::onCursorLeave(const SignalCursorLeave::slot_type& handler)
{
  return std::make_unique<SignalConnection>(sigCursorLeave_.connect(handler));
}

// -----------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::resizeCallback(GLFWwindow* window, int width, int height)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigResize_(width, height);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigKeyboard_(key, scancode, action, mods);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::characterCallback(GLFWwindow* window, unsigned int codepoint)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigCharacter_(codepoint);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::cursorMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigCursorMove_(xpos, ypos);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigMouseButton_(button, action, mods);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::cursorLeaveCallback(GLFWwindow* window, int entered)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigCursorLeave_(entered);
  }
}

} // !namespace vk
} // !namespace yaga

#include "precompiled.h"
#include "event_dispatcher.h"

namespace yaga {
namespace vk {

// -------------------------------------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::resizeCallback(GLFWwindow* window, int width, int height)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigResize_(width, height);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigKeyboard_(key, scancode, action, mods);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::characterCallback(GLFWwindow* window, unsigned int codepoint)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigCharacter_(codepoint);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::cursorMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigCursorMove_(xpos, ypos);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigMouseButton_(button, action, mods);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void EventDispatcher::cursorLeaveCallback(GLFWwindow* window, int entered)
{
  auto dispatcher = reinterpret_cast<EventDispatcher*>(glfwGetWindowUserPointer(window));
  if (dispatcher->window_ == window) {
    dispatcher->sigCursorLeave_(entered);
  }
}

} // !namespace vk
} // !namespace yaga

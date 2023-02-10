#include "precompiled.h"
#include "vulkan_renderer/input.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
VkInput::VkInput(GLFWwindow* window, EventDispatcher* dispatcher) :
  window_(window), dispatcher_(dispatcher)
{
  namespace ph = std::placeholders;
  connections_.push_back(dispatcher_->onKeyboard   (std::bind(&VkInput::onKeyboard,    this, ph::_1, ph::_2, ph::_3, ph::_4)));
  connections_.push_back(dispatcher_->onCharacter  (std::bind(&VkInput::onCharacter,   this, ph::_1                        )));
  connections_.push_back(dispatcher_->onMouseButton(std::bind(&VkInput::onMouseButton, this, ph::_1, ph::_2, ph::_3        )));
  connections_.push_back(dispatcher_->onCursorMove (std::bind(&VkInput::onCursorMove,  this, ph::_1, ph::_2                )));
  connections_.push_back(dispatcher_->onCursorLeave(std::bind(&VkInput::onCursorLeave, this, ph::_1                        )));

  for (int i = 0; i < KEYS_NUMBER; ++i) {
    buffer_.pressedKeys[i]      = 0;
    buffer_.releasedKeys[i]     = 0;
    lastBuffer_.pressedKeys[i]  = 0;
    lastBuffer_.releasedKeys[i] = 0;
  }
  for (int i = 0; i < MOUSE_BUTTONS_NUMBER; ++i) {
    buffer_.pressedMouseButtons[i]      = 0;
    buffer_.releasedMouseButtons[i]     = 0;
    lastBuffer_.pressedMouseButtons[i]  = 0;
    lastBuffer_.releasedMouseButtons[i] = 0;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
VkInput::~VkInput()
{
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::updateState()
{
  updateKeyboardState();
  updateMouseState();
  updateCursorState();
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::updateKeyboardState()
{
  for (int i = 0; i < KEYS_NUMBER; ++i) {
    auto pressed = buffer_.pressedKeys[i].load();
    auto released = buffer_.releasedKeys[i].load();
    state_.keys[i].pressed     = pressed  > lastBuffer_.releasedKeys[i];
    state_.keys[i].wasPressed  = pressed  > lastBuffer_.pressedKeys[i];
    state_.keys[i].wasReleased = released > lastBuffer_.releasedKeys[i];
    // decreasing counters to avoid overflow
    auto prev_released = lastBuffer_.releasedKeys[i].load();
    buffer_.pressedKeys[i]  -= prev_released;
    buffer_.releasedKeys[i] -= prev_released;
    lastBuffer_.pressedKeys[i]  = pressed  - prev_released;
    lastBuffer_.releasedKeys[i] = released - prev_released;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::updateMouseState()
{
  for (int i = 0; i < MOUSE_BUTTONS_NUMBER; ++i) {
    auto pressed  = buffer_.pressedMouseButtons[i].load();
    auto released = buffer_.releasedMouseButtons[i].load();
    state_.mouseButtons[i].pressed     = pressed  > lastBuffer_.releasedMouseButtons[i];
    state_.mouseButtons[i].wasPressed  = pressed  > lastBuffer_.pressedMouseButtons[i];
    state_.mouseButtons[i].wasReleased = released > lastBuffer_.releasedMouseButtons[i];
    // decreasing counters to avoid overflow
    auto prev_released = lastBuffer_.releasedMouseButtons[i].load();
    buffer_.pressedMouseButtons[i]  -= prev_released;
    buffer_.releasedMouseButtons[i] -= prev_released;
    lastBuffer_.pressedMouseButtons[i]  = pressed  - prev_released;
    lastBuffer_.releasedMouseButtons[i] = released - prev_released;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::updateCursorState()
{
  state_.cursorOut = buffer_.cursorOut.load();
  state_.cursorPosition[0] = buffer_.cursorX.load();
  state_.cursorPosition[1] = buffer_.cursorY.load();
  lastBuffer_.cursorOut = state_.cursorOut;
  lastBuffer_.cursorX   = state_.cursorPosition[0];
  lastBuffer_.cursorX   = state_.cursorPosition[1];
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::setCursorMode(CursorMode mode)
{
  switch (mode) {
  case CursorMode::Normal:
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case CursorMode::Hidden:
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
      glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    break;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::onKeyboard(int key, int /*scancode*/, int action, int /*mods*/)
{
  if (key < 0 || key >= KEYS_NUMBER) return;
  switch (action) {
  case GLFW_PRESS:
    buffer_.pressedKeys[key] += 1;
    break;
  case GLFW_RELEASE:
    buffer_.releasedKeys[key] += 1;
    break;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::onCharacter(unsigned int /*codepoint*/)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::onMouseButton(int button, int action, int /*mods*/)
{
  if (button < 0 || button >= MOUSE_BUTTONS_NUMBER) return;
  switch (action) {
  case GLFW_PRESS:
    buffer_.pressedMouseButtons[button] += 1;
    break;
  case GLFW_RELEASE:
    buffer_.releasedMouseButtons[button] += 1;
    break;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::onCursorMove(double xpos, double ypos)
{
  buffer_.cursorX = static_cast<float>(xpos);
  buffer_.cursorY = static_cast<float>(ypos);
}

// -----------------------------------------------------------------------------------------------------------------------------
void VkInput::onCursorLeave(int entered)
{
  buffer_.cursorOut = entered <= 0;
}

} // !namespace vk
} // !namespace yaga

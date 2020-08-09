#include "precompiled.h"
#include "input.h"

namespace yaga {
namespace vk {

// -------------------------------------------------------------------------------------------------------------------------
Input::Input(GLFWwindow* window, EventDispatcher* dispatcher) :
  window_(window), dispatcher_(dispatcher)
{
  namespace ph = std::placeholders;
  keyboardConnection_     = dispatcher_->onKeyboard   (std::bind(&Input::onKeyboard,    this, ph::_1, ph::_2, ph::_3, ph::_4));
  characterConnection_    = dispatcher_->onCharacter  (std::bind(&Input::onCharacter,   this, ph::_1                        ));
  mouseButtonConnection_  = dispatcher_->onMouseButton(std::bind(&Input::onMouseButton, this, ph::_1, ph::_2, ph::_3        ));
  cursorMoveConnection_   = dispatcher_->onCursorMove (std::bind(&Input::onCursorMove,  this, ph::_1, ph::_2                ));
  cursorLeaveConnection_  = dispatcher_->onCursorLeave(std::bind(&Input::onCursorLeave, this, ph::_1                        ));
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

// -------------------------------------------------------------------------------------------------------------------------
Input::~Input()
{
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  dispatcher_->onResize     (keyboardConnection_   );
  dispatcher_->onCharacter  (characterConnection_  );
  dispatcher_->onMouseButton(mouseButtonConnection_);
  dispatcher_->onCursorMove (cursorMoveConnection_ ); 
  dispatcher_->onCursorLeave(cursorLeaveConnection_);
}

// -------------------------------------------------------------------------------------------------------------------------
void Input::updateState()
{
  updateKeyboardState();
  updateMouseState();
  updateCursorState();
}

// -------------------------------------------------------------------------------------------------------------------------
void Input::updateKeyboardState()
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

// -------------------------------------------------------------------------------------------------------------------------
void Input::updateMouseState()
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

// -------------------------------------------------------------------------------------------------------------------------
void Input::updateCursorState()
{
  state_.cursorOut = buffer_.cursorOut.load();
  state_.cursorPosition[0] = buffer_.cursorX.load();
  state_.cursorPosition[1] = buffer_.cursorY.load();
  lastBuffer_.cursorOut = state_.cursorOut;
  lastBuffer_.cursorX   = state_.cursorPosition[0];
  lastBuffer_.cursorX   = state_.cursorPosition[1];
}

// -------------------------------------------------------------------------------------------------------------------------
void Input::setCursorMode(CursorMode mode)
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

// -------------------------------------------------------------------------------------------------------------------------
void Input::onKeyboard(int key, int /*scancode*/, int action, int /*mods*/)
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

// -------------------------------------------------------------------------------------------------------------------------
void Input::onCharacter(unsigned int /*codepoint*/)
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Input::onMouseButton(int button, int action, int /*mods*/)
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

// -------------------------------------------------------------------------------------------------------------------------
void Input::onCursorMove(double xpos, double ypos)
{
  buffer_.cursorX = static_cast<float>(xpos);
  buffer_.cursorY = static_cast<float>(ypos);
}

// -------------------------------------------------------------------------------------------------------------------------
void Input::onCursorLeave(int entered)
{
  buffer_.cursorOut = entered <= 0;
}

} // !namespace vk
} // !namespace yaga

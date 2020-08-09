#ifndef YAGA_VULKAN_RENDERER_SRC_INPUT
#define YAGA_VULKAN_RENDERER_SRC_INPUT

#include <array>
#include <thread>

#include "event_dispatcher.h"
#include "vulkan.h"
#include "engine/input.h"

namespace yaga {
namespace vk {

// -------------------------------------------------------------------------------------------------------------------------
class Input : public yaga::Input
{
public:
  explicit Input(GLFWwindow* window, EventDispatcher* dispatcher);
  virtual ~Input();
  void updateState();
  void setCursorMode(CursorMode mode) override;
  State getState() const override { return state_; }

private:
  void onKeyboard(int key, int scancode, int action, int mods);
  void onCharacter(unsigned int codepoint);
  void onMouseButton(int button, int action, int mods);
  void onCursorMove(double xpos, double ypos);
  void onCursorLeave(int entered);
  void updateKeyboardState();
  void updateMouseState();
  void updateCursorState();

private:
  struct Buffer
  {
    std::array<std::atomic_uint64_t, KEYS_NUMBER> pressedKeys;
    std::array<std::atomic_uint64_t, KEYS_NUMBER> releasedKeys;
    std::array<std::atomic_uint64_t, MOUSE_BUTTONS_NUMBER> pressedMouseButtons;
    std::array<std::atomic_uint64_t, MOUSE_BUTTONS_NUMBER> releasedMouseButtons;
    std::atomic<float> cursorX;
    std::atomic<float> cursorY;
    std::atomic_bool cursorOut;
  };

private:
  GLFWwindow* window_;
  EventDispatcher* dispatcher_;
  EventDispatcher::Connection keyboardConnection_;
  EventDispatcher::Connection characterConnection_;
  EventDispatcher::Connection mouseButtonConnection_;
  EventDispatcher::Connection cursorMoveConnection_;
  EventDispatcher::Connection cursorLeaveConnection_;
  State state_;
  Buffer buffer_;
  Buffer lastBuffer_; // it is not necessary for lastBuffer_ to be atomic... 
};

typedef std::unique_ptr<Input> InputPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_INPUT

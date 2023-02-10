#ifndef YAGA_VULKAN_RENDERER_INPUT
#define YAGA_VULKAN_RENDERER_INPUT

#include <array>
#include <thread>

#include "vulkan_renderer/event_dispatcher.h"
#include "vulkan_renderer/vulkan.h"
#include "engine/input.h"
#include "utility/signal.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
class VkInput : public Input
{
public:
  explicit VkInput(GLFWwindow* window, EventDispatcher* dispatcher);
  virtual ~VkInput();
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
  State state_;
  Buffer buffer_;
  Buffer lastBuffer_; // it is not necessary for lastBuffer_ to be atomic... 
  SignalConnections connections_;
};

typedef std::unique_ptr<VkInput> VkInputPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_INPUT

#ifndef YAGA_ENGINE_INPUT
#define YAGA_ENGINE_INPUT

#include <array>
#include <memory>

#include "input_keys.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
class Input
{
public:
  enum class CursorMode
  {
    Normal,
    Hidden
  };
  struct KeyState
  {
    bool wasPressed;
    bool wasReleased;
    bool pressed;
  };
  struct State
  {
    std::array<KeyState, KEYS_NUMBER> keys;
    std::array<KeyState, MOUSE_BUTTONS_NUMBER> mouseButtons;
    std::array<float, 2> cursorPosition;
    bool cursorOut;
  };

public:
  virtual void setCursorMode(CursorMode mode) = 0;
  virtual State getState() const = 0;
};

typedef std::unique_ptr<Input> InputPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_INPUT

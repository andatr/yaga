#ifndef YAGA_VULKAN_RENDERER_SRC_EVENT_DISPATCHER
#define YAGA_VULKAN_RENDERER_SRC_EVENT_DISPATCHER

#include <memory>
#include <boost/signals2.hpp>

#include "vulkan.h"

namespace yaga {
namespace vk {

class EventDispatcher
{
public:
  typedef boost::signals2::connection Connection;
  typedef boost::signals2::signal<void(int, int)> SignalResize;
  typedef boost::signals2::signal<void(int, int, int, int)> SignalKeyboard;
  typedef boost::signals2::signal<void(unsigned int)> SignalCharacter;
  typedef boost::signals2::signal<void(int, int, int)> SignalMouseButton;
  typedef boost::signals2::signal<void(double, double)> SignalCursorMove;
  typedef boost::signals2::signal<void(int)> SignalCursorLeave;

public:
  EventDispatcher(GLFWwindow* window);
  Connection onResize       (const SignalResize::slot_type&      handler) { return sigResize_.connect(handler);      }
  Connection onKeyboard     (const SignalKeyboard::slot_type&    handler) { return sigKeyboard_.connect(handler);    }
  Connection onCharacter    (const SignalCharacter::slot_type&   handler) { return sigCharacter_.connect(handler);   }
  Connection onMouseButton  (const SignalMouseButton::slot_type& handler) { return sigMouseButton_.connect(handler); }
  Connection onCursorMove   (const SignalCursorMove::slot_type&  handler) { return sigCursorMove_.connect(handler);  }
  Connection onCursorLeave  (const SignalCursorLeave::slot_type& handler) { return sigCursorLeave_.connect(handler); }
   
  void onKeyboard   (const Connection& handler) { return sigKeyboard_.disconnect(handler);    }
  void onCharacter  (const Connection& handler) { return sigCharacter_.disconnect(handler);   }
  void onMouseButton(const Connection& handler) { return sigMouseButton_.disconnect(handler); }
  void onCursorMove (const Connection& handler) { return sigCursorMove_.disconnect(handler);  }
  void onCursorLeave(const Connection& handler) { return sigCursorLeave_.disconnect(handler); }

private:
  static void resizeCallback     (GLFWwindow* window, int width, int height);
  static void keyboardCallback   (GLFWwindow* window, int key, int scancode, int action, int mods);
  static void characterCallback  (GLFWwindow* window, unsigned int codepoint);
  static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  static void cursorMoveCallback (GLFWwindow* window, double xpos, double ypos);
  static void cursorLeaveCallback(GLFWwindow* window, int entered);

private:
  GLFWwindow* window_;
  SignalResize sigResize_;
  SignalKeyboard sigKeyboard_;
  SignalCharacter sigCharacter_;
  SignalMouseButton sigMouseButton_;
  SignalCursorMove sigCursorMove_;
  SignalCursorLeave sigCursorLeave_;
};

typedef std::unique_ptr<EventDispatcher> EventDispatcherPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_EVENT_DISPATCHER

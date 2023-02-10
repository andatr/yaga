#ifndef YAGA_GUI_CONTEXT_APP
#define YAGA_GUI_CONTEXT_APP

#include "utility/compiler.h"

DISABLE_WARNINGS
#include <boost/asio.hpp>
ENABLE_WARNINGS

#include "imgui_wrapper.h"
#include "gui/dispatcher.h"

namespace yaga {

class GuiContextApp
{
public:
  virtual ImVec2 screenSize() = 0;
  virtual Dispatcher& dispatcher() = 0;
  virtual boost::asio::thread_pool& taskPool() = 0;
};

} // !namespace yaga

#endif // !YAGA_GUI_CONTEXT_APP

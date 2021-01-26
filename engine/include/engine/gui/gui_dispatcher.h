#ifndef YAGA_ENGINE_GUI_DISPATCHER
#define YAGA_ENGINE_GUI_DISPATCHER

#include <boost/asio.hpp>

namespace yaga {

class GuiDispatcher
{
public:
  GuiDispatcher();
  void process();
  template <typename Handler>
  void post(Handler handler);

private:
  size_t current_;
  size_t next_;
  boost::asio::io_context dispatchers_[2];
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Handler>
void GuiDispatcher::post(Handler handler)
{
  dispatchers_[next_].post(handler);
}

} // !namespace yaga

#endif // !YAGA_ENGINE_GUI_DISPATCHER

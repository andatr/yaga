#ifndef YAGA_GUI_DISPATCHER
#define YAGA_GUI_DISPATCHER

#include "utility/compiler.h"

DISABLE_WARNINGS
#include <boost/asio.hpp>
ENABLE_WARNINGS

namespace yaga {

class Dispatcher
{
public:
  Dispatcher();
  void process();
  template <typename Handler>
  void post(Handler handler);
  void reset();

private:
  size_t current_;
  size_t next_;
  boost::asio::io_context dispatchers_[2];
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Handler>
void Dispatcher::post(Handler handler)
{
  dispatchers_[next_].post(std::move(handler));
}

} // !namespace yaga

#endif // !YAGA_GUI_DISPATCHER

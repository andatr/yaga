#include "precompiled.h"
#include "gui/gui_context.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
GuiDispatcher::GuiDispatcher() :
  current_(0),
  next_(1)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void GuiDispatcher::process()
{
  auto& current = dispatchers_[current_];
  current.poll();
  if (current.stopped()) {
    current.restart();
  }
  current_ = (current_ + 1) % 2;
  next_    = (next_    + 1) % 2;
}

} // !namespace yaga

#include "precompiled.h"
#include "gui/context.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
Dispatcher::Dispatcher() :
  current_(0),
  next_(1)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Dispatcher::process()
{
  auto& current = dispatchers_[current_];
  current.poll();
  if (current.stopped()) {
    current.restart();
  }
  current_ = (current_ + 1) % 2;
  next_    = (next_    + 1) % 2;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Dispatcher::reset()
{
  dispatchers_[0].restart();
  dispatchers_[1].restart();
}

} // !namespace yaga

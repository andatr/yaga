#include "precompiled.h"
#include "gui/gui_context.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
GuiContext::GuiContext(IGuiContext* gui) :
  gui_(gui)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ImVec2 GuiContext::screen() const
{
  return gui_->screenSize();
}

} // !namespace yaga

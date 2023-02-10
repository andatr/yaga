#include "precompiled.h"
#include "gui/context.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
GuiContext::GuiContext(GuiContextApp* app, DockingContext* docking) :
  app_(app),
  docking_(docking)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ImVec2 GuiContext::screen() const
{
  return app_->screenSize();
}

} // !namespace yaga

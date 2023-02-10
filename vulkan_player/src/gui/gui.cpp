#include "precompiled.h"
#include "gui.h"
#include "main_menu_widget.h"
#include "player_widget.h"

namespace yaga {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
void renderMenu(GuiContext context, GuiApp* app)
{
  auto menu = std::make_shared<MainMenuWidget>(context, app);
  menu->docking(Docking::Menu);
  menu->enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void renderPlayerControls(GuiContext context, GuiApp* app)
{
  auto player = std::make_shared<PlayerWidget>(context, app->player());
  player->docking(Docking::Bottom);
  player->enqueueRendering();
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
void renderGui(GuiContext context, GuiApp* app)
{
  renderMenu          (context, app);
  renderPlayerControls(context, app);
}

} // !namespace yaga

#include "precompiled.h"
#include "gui.h"
#include "asset_view.h"
#include "main_menu.h"
#include "gui/property_viewer.h"
#include "gui/popup.h"

namespace yaga {
namespace editor {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
void renderAssetView(GuiContext context, AssetManager* assetManager)
{
  auto assetView = std::make_shared<AssetView>(context, assetManager);
  assetView->enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void renderPropertyEditor(GuiContext context)
{
  auto editor = std::make_unique<PropertyViewer>(context);
  editor->docking(Docking::Right);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Gui::Gui(GuiContext context, GuiApp* app) :
  context_(context),
  popupVisible_(false)
{
  createMenu(app);
  renderAssetView(context, app->assetManager());
  renderPropertyEditor(context);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::createMenu(GuiApp* app)
{
  menu_ = std::make_shared<MainMenu>(context_, app);
  menu_->docking(Docking::Menu);
  menu_->enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::showPopup(const std::string& content)
{
  popupVisible_ = true;
  //popup::render(context_, "##GeneralPopup", content, [this]() { return popupVisible_.load(); });
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::hidePopup()
{
  popupVisible_ = false;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::exit()
{
  menu_->exit();
}

} // !namespace editor
} // !namespace yaga

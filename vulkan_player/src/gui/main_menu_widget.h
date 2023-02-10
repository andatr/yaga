#ifndef YAGA_VULKAN_PLAYER_SRC_GUI_MAIN_MENU_WIDGET
#define YAGA_VULKAN_PLAYER_SRC_GUI_MAIN_MENU_WIDGET

#include "gui.h"
#include "gui/widget.h"

namespace yaga {

class MainMenuWidget : public Widget
{
public:
  MainMenuWidget(GuiContext context, GuiApp* app);
  void render() override;

private:
  void open();
  void exit();
  void renderFileInfo();
  void renderStreamInfo(const char* label, StreamFamily* info);

private:
  GuiApp* app_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_GUI_MAIN_MENU_WIDGET

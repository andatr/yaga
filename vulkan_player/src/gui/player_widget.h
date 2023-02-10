#ifndef YAGA_VULKAN_PLAYER_SRC_GUI_PLAYER_WIDGET
#define YAGA_VULKAN_PLAYER_SRC_GUI_PLAYER_WIDGET

#include <memory>

#include "player.h"
#include "gui/widget.h"

namespace yaga {

// ----------------------------------------------------------------------------------------------------------------------------- 
class PlayerWidget : public Widget
{
public:
  PlayerWidget(GuiContext context, Player* player);
  void render() override;

private:
  Player* player_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_GUI_PLAYER_WIDGET

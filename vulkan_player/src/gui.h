#ifndef YAGA_VULKAN_PLAYER_SRC_GUI
#define YAGA_VULKAN_PLAYER_SRC_GUI

#include <memory>
#include <string>

#include "player.h"
#include "gui/context.h"
#include "gui/dispatcher.h"

namespace yaga {

class GuiApp
{
public:
  virtual ~GuiApp() {};
  virtual void open(const std::string& filename) = 0;
  virtual void exit() = 0;
  virtual Player* player() = 0;
};

void renderGui(GuiContext context, GuiApp* app);

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_GUI

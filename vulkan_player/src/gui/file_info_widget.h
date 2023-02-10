#ifndef YAGA_VULKAN_PLAYER_SRC_GUI_FILE_INFO_WIDGET
#define YAGA_VULKAN_PLAYER_SRC_GUI_FILE_INFO_WIDGET

#include <vector>
#include <string>
#include <utility>

#include "player.h"
#include "gui/widget.h"

namespace yaga {

// ----------------------------------------------------------------------------------------------------------------------------- 
class FileInfoWidget : public Widget
{
public:
  FileInfoWidget(GuiContext context, Player* player);
  void render() override;

private:
  struct StreamInfo
  {
    std::string title;
    std::vector<std::pair<std::string, std::string>> description;
  };

private:
  void buildStreamInfo(StreamFamily* info, const char* name);

private:
  Player* player_;
  std::vector<StreamInfo> streams_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_GUI_FILE_INFO_WIDGET

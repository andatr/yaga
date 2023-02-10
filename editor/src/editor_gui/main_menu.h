#ifndef YAGA_EDITOR_SRC_EDITOR_GUI_MAIN_MENU
#define YAGA_EDITOR_SRC_EDITOR_GUI_MAIN_MENU

#include <memory>
#include <functional>
#include <string>

#include "gui.h"
#include "gui/widget.h"

namespace yaga {
namespace editor {

class MainMenu : public Widget
{
public:
  MainMenu(GuiContext context, GuiApp* app);
  void exit();
  void render() override;

private:
  void open();
  void save  (Func next = []() {});
  void saveAs(Func next = []() {});
  void save  (const std::string& filename, Func next);
  void confirmSave(Func = []() {});

private:
  GuiApp* app_;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_GUI_MAIN_MENU

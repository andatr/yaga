#ifndef YAGA_EDITOR_SRC_EDITOR_GUI_GUI
#define YAGA_EDITOR_SRC_EDITOR_GUI_GUI

#include <atomic>
#include <memory>
#include <string>

#include "asset_manager/asset_manager.h"
#include "gui/context.h"

namespace yaga {
namespace editor {

typedef std::function<void()> Func;

class GuiApp
{
public:
  typedef boost::signals2::signal<void()> SignalExit;

public:
  virtual ~GuiApp() {};
  virtual const std::string& filename() const = 0;
  virtual bool hasChanges() const = 0;
  virtual void hasChanges(bool value) = 0;
  virtual void save(const std::string& filename, Func next) = 0;
  virtual void open(const std::string& filename) = 0;
  virtual void exit() = 0;
  virtual AssetManager* assetManager() = 0;
};

class MainMenu;
typedef std::shared_ptr<MainMenu> MainMenuPtr;

class Gui
{
public:
  Gui(GuiContext context, GuiApp* app);
  void showPopup(const std::string& content);
  void hidePopup();
  void exit();

private:
  void createMenu(GuiApp* app);

private:
  GuiContext context_;
  std::atomic_bool popupVisible_;
  MainMenuPtr menu_;
};

typedef std::unique_ptr<Gui> GuiPtr;

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_EDITOR_GUI_GUI
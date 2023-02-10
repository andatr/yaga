#ifndef YAGA_EDITOR_SRC_APPLICATION
#define YAGA_EDITOR_SRC_APPLICATION

#include <string>
#include <memory>

#include "utility/compiler.h"
DISABLE_WARNINGS
#include <boost/asio.hpp>
ENABLE_WARNINGS

#include "editor_gui/gui.h"
#include "asset_manager/asset_manager.h"
#include "engine/basic_application.h"
#include "gui/dispatcher.h"
#include "gui/context.h"
#include "gui/property_viewer.h"

namespace yaga {
namespace editor {

class Application :
  public BasicApplication,
  public GuiContextApp,
  public GuiApp
{
public:
  Application(ConfigPtr config, const std::string& projectPath);
  virtual ~Application();
  void init(Context* context, Input* input) override;
  void resize()   override;
  bool loop()     override;
  void stop()     override;
  void shutdown() override;
  void gui()      override;
  // GuiContextApp
  ImVec2 screenSize() override;
  Dispatcher& dispatcher() override;
  boost::asio::thread_pool& taskPool() override;
  // GuiApp
  const std::string& filename() const override;
  AssetManager* assetManager() override;
  bool hasChanges() const override;
  void hasChanges(bool value) override;
  void save(const std::string& filename, Func next) override;
  void open(const std::string& filename) override;
  void exit() override;

private:
  typedef BasicApplication base;

private:
  boost::asio::thread_pool taskPool_;
  DockingContext docking_;
  GuiContext guiContext_;
  Dispatcher dispatcher_;
  bool hasChanges_;
  std::string filename_;
  AssetManagerPtr assetManager_;
  GuiPtr gui_;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_APPLICATION

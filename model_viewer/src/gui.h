#ifndef YAGA_MODEL_VIEWER_SRC_GUI
#define YAGA_MODEL_VIEWER_SRC_GUI

#include <functional>
#include <memory>
#include <string>
#include <boost/property_tree/ptree_fwd.hpp>

#include "engine/gui/gui_context.h"

namespace yaga {
namespace mview {

class Application;

class Gui : public IGuiContext
{
public:
  explicit Gui(const boost::property_tree::ptree& options, Application* app);
  virtual ~Gui();
  void render();
  void resize();
  void exit();
  // IGuiContext
  ImVec2 screenSize() override;
  GuiDispatcher& guiDispatcher() override;
  boost::asio::thread_pool& taskPool() override;

private:
  void onMenuOpenClick();
  void onMenuSaveClick(std::function<void()> next = []() {});
  void onMenuSaveAsClick(std::function<void()> next = []() {});
  void onMenuExitClick();
  void onHasChanges(std::function<void()> next = []() {});

private:
  GuiContext context_;
  bool hasChanges_;
  std::string workingDir_;
  std::string filename_;
  Application* app_;
  GuiDispatcher dispatcher_;
};

} // !namespace mview
} // !namespace yaga

#endif // !YAGA_MODEL_VIEWER_SRC_GUI

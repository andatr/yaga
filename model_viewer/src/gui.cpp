#include "precompiled.h"
#include "gui.h"
#include "application.h"
#include "engine/gui/file_dialog.h"
#include "engine/gui/popup_dialog.h"

namespace yaga {
namespace mview {
namespace {

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Gui::Gui(const boost::property_tree::ptree& options, Application* app) :
  context_(this),
  hasChanges_(true),
  workingDir_(options.get<std::string>("workingDir")),
  app_(app)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Gui::~Gui()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ImVec2 Gui::screenSize()
{
  return app_->context_->resolution();
}

// -----------------------------------------------------------------------------------------------------------------------------
GuiDispatcher& Gui::guiDispatcher()
{
  return dispatcher_;
}

// -----------------------------------------------------------------------------------------------------------------------------
boost::asio::thread_pool& Gui::taskPool()
{
  return app_->taskPool_;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::render()
{
  dispatcher_.process();
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open"))
        onMenuOpenClick();
      if (ImGui::MenuItem("Save"))
        onMenuSaveClick();
      if (ImGui::MenuItem("Save As"))
        onMenuSaveAsClick();
      ImGui::Separator();
      if (ImGui::MenuItem("Exit"))
        onMenuExitClick();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      ImGui::MenuItem("About");
      ImGui::MenuItem("Debug");
      ImGui::MenuItem("ImGui Demo");
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  //if (!showDemo_) return;
  //ImGui::ShowDemoWindow(&showDemo_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::resize()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::exit()
{
  onMenuExitClick();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::onMenuOpenClick()
{
  if (hasChanges_) {
    onHasChanges(std::bind(&Gui::onMenuOpenClick, this));
  }
  else {
    auto callback = [this](const std::string& filename) {
      filename_ = filename;
      auto show = std::make_shared<bool>(true);
      renderPopupDialog(context_, "##PopupLoading", "Loading...", show);
      context_.post(std::bind(&Application::open, app_, filename_), [show]() { *show = false; });
    };
    renderFileDialog(context_, FileDialogMode::Open, workingDir_, callback);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::onMenuSaveClick(std::function<void()> next)
{
  if (filename_.empty()) {
    onMenuSaveAsClick(next);
  }
  else {
    hasChanges_ = false;
    auto show = std::make_shared<bool>(true);
    renderPopupDialog(context_, "##PopupSaving", "Saving...", show);
    context_.post(std::bind(&Application::save, app_, filename_), [show, next]() {
      *show = false;
      next();
    });
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::onMenuSaveAsClick(std::function<void()> next)
{
  auto callback = [this, next](const std::string& filename) {
    filename_ = filename;
    onMenuSaveClick(next);
  };
  renderFileDialog(context_, FileDialogMode::Save, workingDir_, callback);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::onMenuExitClick()
{
  if (hasChanges_)
    onHasChanges(std::bind(&Gui::onMenuExitClick, this));
  else
    app_->exit();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Gui::onHasChanges(std::function<void()> next)
{
  auto callback = [this, next](DialogResult result) {
    if (result == DialogResult::Yes) {
      hasChanges_ = false;
      onMenuSaveClick(next);
    }
    else if (result == DialogResult::No) {
      hasChanges_ = false;
      next();
    }
  };
  renderPopupDialog(context_, "##PopupSave", "Save changes?", DialogResult::YesNoCancel, callback);
  return;
}

} // !namespace mview
} // !namespace yaga

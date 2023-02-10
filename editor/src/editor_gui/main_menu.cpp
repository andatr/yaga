#include "precompiled.h"
#include "main_menu.h"
#include "gui/imgui_demo.h"
#include "gui/file_dialog.h"
#include "gui/popup.h"
#include "gui/widget.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
  MainMenu::MainMenu(GuiContext context, GuiApp* app) :
  Widget(context),
  app_(app)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenu::render()
{
  setPosition();
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open")) {
        open();
      }
      if (ImGui::MenuItem("Save")) {
        save();
      }
      if (ImGui::MenuItem("Save As")) {
        saveAs();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Exit")) {
        exit();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
      ImGui::MenuItem("Debug");
      if (ImGui::MenuItem("ImGui Demo")) {
        renderImguiDemo(context_);
      }
      ImGui::EndMenu();
    }
    updatePosition();
    ImGui::EndMainMenuBar();
  }
  enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenu::open()
{
  if (app_->hasChanges()) {
    confirmSave([this]() { open(); });
  }
  else {
    file_dialog::render(context_, file_dialog::Mode::Open, std::bind(&GuiApp::open, app_, std::placeholders::_1));
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenu::save(Func next)
{
  if (app_->filename().empty()) {
    saveAs(next);
  }
  else {
    save(app_->filename(), next);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenu::save(const std::string& filename, Func next)
{
  app_->save(filename, next);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenu::saveAs(Func next)
{
  file_dialog::render(context_, file_dialog::Mode::Save, [this, next](const std::string& filename) {
    save(filename, next);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenu::exit()
{
  if (app_->hasChanges()) {
    confirmSave([this]() { exit(); });
  }
  else {
    app_->exit();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenu::confirmSave(Func next)
{
  auto callback = [this, next](popup::Result result) {
    if (result == popup::Result::Yes) {
      app_->hasChanges(false);
      save(next);
    }
    else if (result == popup::Result::No) {
      app_->hasChanges(false);
      next();
    }
  };
  popup::render(context_, "##PopupSave", "Save changes?", popup::Result::YesNoCancel, callback);
  return;
}

} // !namespace editor
} // !namespace yaga

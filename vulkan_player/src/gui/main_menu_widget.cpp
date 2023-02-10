#include "precompiled.h"
#include "main_menu_widget.h"
#include "file_info_widget.h"
#include "gui/imgui_demo.h"
#include "gui/file_dialog.h"
#include "gui/popup.h"
#include "gui/widget.h"
#include "gui/imgui_demo.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
MainMenuWidget::MainMenuWidget(GuiContext context, GuiApp* app) :
  Widget(context),
  app_(app)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenuWidget::render()
{
  setPosition();
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open")) {
        open();
      }
      renderFileInfo();
      ImGui::Separator();
      if (ImGui::MenuItem("Exit")) {
        exit();
      }
      ImGui::EndMenu();
    }
    renderStreamInfo("Video",    app_->player()->videoStreams());
    renderStreamInfo("Audio",    app_->player()->audioStreams());
    renderStreamInfo("Subtitle", app_->player()->subtitleStreams());
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Demo")) {
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
void MainMenuWidget::open()
{
  auto callback = [this](const std::string& filename) {
    context_.postSeq(std::bind(&GuiApp::open, app_, filename));
  };
  file_dialog::render(context_, file_dialog::Mode::Open, callback);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenuWidget::exit()
{
  context_.postSeq(std::bind(&GuiApp::exit, app_));
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenuWidget::renderFileInfo()
{
  if (!ImGui::MenuItem("Info")) return;
  auto fi = std::make_shared<FileInfoWidget>(context_, app_->player());
  fi->enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void MainMenuWidget::renderStreamInfo(const char* label, StreamFamily* info)
{
  if (info->count() == 0) return;
  if (!ImGui::BeginMenu(label)) return;
  for (int i = 0; i < info->count(); ++i) {
    bool checked = i == info->current();
    if (ImGui::MenuItem(info->stream(i)->name().c_str(), nullptr, checked) && !checked) {
      context_.postSeq([info, i]() { info->activateStream(i); });
    }
  }
  ImGui::EndMenu();
}

} // !namespace yaga

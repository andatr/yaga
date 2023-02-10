#include "precompiled.h"
#include "rename_dialog.h"
#include "gui/popup.h"
#include "gui/widget.h"

#define ID(str) "##RenameDialog_"##str
#define LABEL(str) str##"##RenameDialog"

namespace yaga {
namespace rename {
namespace {

constexpr float buttonWidth = 80.0f;

class RenameDialogImpl : public Widget
{
public:
  RenameDialogImpl(GuiContext context, const std::string& title, const std::string& value, Handle handle);
  void render() override;

private:
  std::string title_;
  Handle handle_;
  std::vector<char> buffer_;
  ImVec2 size_;
};

// -----------------------------------------------------------------------------------------------------------------------------
RenameDialogImpl::RenameDialogImpl(GuiContext context, const std::string& title, const std::string& value, Handle handle) :
  Widget(context),
  title_(title),
  handle_(handle),
  buffer_(512),
  size_{}
{
  memcpy(buffer_.data(), value.c_str(), std::min(buffer_.size(), value.size()));
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenameDialogImpl::render()
{
  auto result = popup::Result::None;
  ImGui::OpenPopup(ID("Widget"));
  ImGui::SetNextWindowPos((context_.screen() - size_) / 2.0f);
  if (!ImGui::BeginPopup(ID("Widget"))) {
    enqueueRendering();
    return;
  }
  ImGui::Text(title_.c_str());
  if (ImGui::InputText(ID("NewValue"), buffer_.data(), buffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
    result = popup::Result::Yes;
  }
  if (ImGui::Button(LABEL("Ok"), ImVec2(buttonWidth, 0))) {
    result = popup::Result::Yes;
  }
  ImGui::SameLine();
  if (ImGui::Button(LABEL("Cancel"), ImVec2(buttonWidth, 0))) {
    result = popup::Result::No;
  }
  setPosition();
  ImGui::EndPopup();
  if (result == popup::Result::Yes) {
    if (!handle_(buffer_.data())) enqueueRendering();
  }
  else if (result == popup::Result::None) {
    enqueueRendering();
  }
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
void render(GuiContext context, const std::string& title, const std::string& value, Handle handle)
{
  auto impl = std::make_shared<RenameDialogImpl>(context, title, value, handle);
  impl->enqueueRendering();
}

} // !namespace rename
} // !namespace yaga

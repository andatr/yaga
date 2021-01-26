#include "precompiled.h"
#include "gui/popup_dialog.h"

namespace yaga {
namespace {

constexpr float buttonWidth = 80.0f;

struct State
{
  std::shared_ptr<bool> show;
  std::string id;
  std::string content;
  DialogResult buttons;
  ImVec2 size;

  State(const std::string& id, const std::string& content, DialogResult buttons, std::shared_ptr<bool> show);
};

// -----------------------------------------------------------------------------------------------------------------------------
State::State(const std::string& id, const std::string& content, DialogResult buttons, std::shared_ptr<bool> show) :
  show(show),
  id(id),
  content(content),
  buttons(buttons),
  size{}
{
}

typedef std::shared_ptr<State> StatePtr;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename E>
constexpr auto toInt(E e) noexcept
{
  return static_cast<std::underlying_type_t<E>>(e);
}

} // !namespace

class PopupDialogImpl
{
public:
  PopupDialogImpl(GuiContext context, const std::string& id, const std::string& content,
    DialogResult buttons, PopupDialogHandle handle, std::shared_ptr<bool> show);
  void operator()();

private:
  GuiContext context_;
  StatePtr state_;
  PopupDialogHandle handle_;
};

// -----------------------------------------------------------------------------------------------------------------------------
PopupDialogImpl::PopupDialogImpl(GuiContext context, const std::string& id, const std::string& content,
  DialogResult buttons, PopupDialogHandle handle, std::shared_ptr<bool> show) :
    context_(context),
    handle_(handle)
{
  state_ = std::make_shared<State>(id, content, buttons, show);
}

// -----------------------------------------------------------------------------------------------------------------------------
void PopupDialogImpl::operator()()
{
#define RENDER_BUTTON(btn)                                  \
  if (toInt(state_->buttons) & toInt(DialogResult::btn)) {  \
    if (ImGui::Button(#btn, ImVec2(buttonWidth, 0)))        \
      result = DialogResult::btn;                           \
    ImGui::SameLine();                                      \
  }
  DialogResult result = DialogResult::None;
  ImGui::OpenPopup(state_->id.c_str());
  ImGui::SetNextWindowPos((context_.screen() - state_->size) / 2.0f);
  if (!ImGui::BeginPopup(state_->id.c_str())) {
    context_.render(*this);
    return;
  }
  ImGui::Text("%s", state_->content.c_str());
  RENDER_BUTTON(Yes)
  RENDER_BUTTON(No)
  RENDER_BUTTON(Retry)
  RENDER_BUTTON(Cancel)
  state_->size = ImGui::GetWindowSize();
  ImGui::EndPopup();
  if (result == DialogResult::None && (state_->show == nullptr || *state_->show == true))
    context_.render(*this);
  else
    context_.render(std::bind(handle_, result));
#undef RENDER_BUTTON
}

// -----------------------------------------------------------------------------------------------------------------------------
void renderPopupDialog(GuiContext context, const std::string& id, const std::string& content,
  DialogResult buttons, PopupDialogHandle handle)
{
  context.render(PopupDialogImpl(context, id, content, buttons, handle, nullptr));
}

// -----------------------------------------------------------------------------------------------------------------------------
void renderPopupDialog(GuiContext context, const std::string& id, const std::string& content, std::shared_ptr<bool> show)
{
  context.render(PopupDialogImpl(context, id, content, DialogResult::None, [](DialogResult) {}, show));
}

} // !namespace yaga

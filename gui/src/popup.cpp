#include "precompiled.h"
#include "gui/popup.h"
#include "gui/widget.h"

namespace yaga {
namespace popup {
namespace {

constexpr float buttonWidth = 80.0f;

class Popup : public Widget
{
public:
  Popup(
    GuiContext         context,
    const std::string& id,
    const std::string& content,
    Result             buttons,
    Handle             handle,
    ShowHandle         show);
  void render() override;

private:
  std::string id_;
  std::string content_;
  Result buttons_;
  Handle handle_;
  ShowHandle show_;
  ImVec2 size_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename E>
constexpr auto toInt(E e) noexcept
{
  return static_cast<std::underlying_type_t<E>>(e);
}

// -----------------------------------------------------------------------------------------------------------------------------
Popup::Popup(
  GuiContext         context,
  const std::string& id,
  const std::string& content,
  Result             buttons,
  Handle             handle,
  ShowHandle         show
) :
  Widget(context),
  id_(id),
  content_(content),
  buttons_(buttons),
  handle_(handle),
  show_(show),
  size_{}
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Popup::render()
{
/*#define RENDER_BUTTON(btn)                                  \
  if (toInt(buttons_) & toInt(Result::btn)) {               \
    if (ImGui::Button(#btn, ImVec2(buttonWidth, 0)))        \
      result = Result::btn;                                 \
    ImGui::SameLine();                                      \
  }*/
  Result result = Result::None;
  ImGui::OpenPopup(id_.c_str());
  //ImVec2 size(100.0f, 100.0f);
  ImVec2 pos(300.0f, 100.0f);
  ImGui::SetNextWindowPos(pos);


  if (!ImGui::BeginPopup(id_.c_str())) {
    enqueueRendering();
    return;
  }
  ImGui::Text("%s", content_.c_str());
  
  //RENDER_BUTTON(Yes)
  //RENDER_BUTTON(No)
  //RENDER_BUTTON(Retry)
 // RENDER_BUTTON(Cancel)
  ImGui::EndPopup();

  enqueueRendering();

  //size_ = ImGui::GetWindowSize();
  /*if (buttons_ == Result::None) {
    if (show_()) {
      render();
    }
    else {
      ImGui::CloseCurrentPopup();
    }
  }
  else {
    if (result == Result::None) {
      render();
    }
    else {
      ImGui::CloseCurrentPopup();
      context_.postSeq(std::bind(handle_, result));
    }
  }
  ImGui::EndPopup();*/
//#undef RENDER_BUTTON
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
void render(
  GuiContext         context,
  const std::string& id,
  const std::string& content,
  Result             buttons,
  Handle             handle)
{
  auto dialog = std::make_shared<Popup>(context, id, content, buttons, handle, []() { return false; });
  dialog->render();
}

// -----------------------------------------------------------------------------------------------------------------------------
void render(GuiContext context, const std::string& id, const std::string& content, ShowHandle show)
{
  auto dialog = std::make_shared<Popup>(context, id, content, Result::None, [](Result) {}, show);
  dialog->enqueueRendering();
}

} // !namespace popup
} // !namespace yaga

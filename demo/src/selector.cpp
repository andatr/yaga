#include "precompiled.h"
#include "selector.h"
#include "widget.h"

namespace yaga {
namespace demo {
namespace {

constexpr float TOP   = 10.0f;
constexpr float LEFT  = 10.0f;

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
class SelectorImpl : public Widget
{
public:
  SelectorImpl(GuiContext context, SelectorApp* app);
  void render() override;

private:
  SelectorApp* app_;
};

// -----------------------------------------------------------------------------------------------------------------------------
SelectorImpl::SelectorImpl(GuiContext context, SelectorApp* app) :
  Widget(context),
  app_(app)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void SelectorImpl::render()
{
  ImGui::SetNextWindowPos({ LEFT, TOP });
  constexpr auto flag = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
  if (ImGui::Begin("Selector", nullptr, flag)) {
    auto items = app_->demoList();
    int current = app_->currentDemo();
    ImGui::TextUnformatted("Scene");
    ImGui::SetNextItemWidth(WIDGET_WIDTH);
    if (ImGui::Combo("##Selector#Scene", &current, items.data(), items.size())) {
      app_->currentDemo(current);
    }
    app_->drawDemoGui();
  }
  ImGui::End();
  enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
Selector::Selector(GuiContext context, SelectorApp* app)
{
  impl_ = std::make_shared<SelectorImpl>(context, app);
  impl_->enqueueRendering();
}

} // !namespace demo
} // !namespace yaga

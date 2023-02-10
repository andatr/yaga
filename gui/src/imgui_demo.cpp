#include "precompiled.h"
#include "gui/imgui_demo.h"

namespace yaga {
namespace {

class ImguiDemoImpl
{
public:
  ImguiDemoImpl(GuiContext context);
  void operator()();

private:
  GuiContext context_;
};

// -----------------------------------------------------------------------------------------------------------------------------
ImguiDemoImpl::ImguiDemoImpl(GuiContext context) :
  context_(context)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void ImguiDemoImpl::operator()()
{
  bool show = true;
  ImGui::ShowDemoWindow(&show);
  if (show) {
    context_.postSeq(*this);
  }
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
void renderImguiDemo(GuiContext context)
{
  context.postSeq(ImguiDemoImpl(context));
}

} // !namespace yaga

#ifndef YAGA_GUI_CONTEXT
#define YAGA_GUI_CONTEXT

#include "utility/compiler.h"

DISABLE_WARNINGS
#include <boost/asio.hpp>
ENABLE_WARNINGS

#include "imgui_wrapper.h"
#include "gui/docking.h"
#include "gui/context_app.h"

namespace yaga {

class GuiContext
{
public:
  explicit GuiContext(GuiContextApp* app, DockingContext* docking);
  ImVec2 screen() const;
  DockingContext& docking() { return *docking_; }
  template <typename SeqHandler>
  void postSeq(SeqHandler seqHandler);
  template <typename ConHandler>
  void postCon(ConHandler conHandler);
  template <typename ConHandler, typename SeqHandler>
  void postCon(ConHandler conHandler, SeqHandler seqHandler);

private:
  GuiContextApp* app_;
  DockingContext* docking_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename SeqHandler>
void GuiContext::postSeq(SeqHandler handler)
{
  app_->dispatcher().post(handler);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename ConHandler>
void GuiContext::postCon(ConHandler conHandler)
{
  boost::asio::post(app_->taskPool(), std::move(conHandler));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename ConHandler, typename SeqHandler>
void GuiContext::postCon(ConHandler conHandler, SeqHandler seqHandler)
{
  boost::asio::post(app_->taskPool(), [app = app_, con = std::move(conHandler), seq = std::move(seqHandler)]() {
    con();
    app->dispatcher().post(seq);
  });
}

} // !namespace yaga

#endif // !v

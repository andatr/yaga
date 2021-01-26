#ifndef YAGA_ENGINE_GUI_CONTEXT
#define YAGA_ENGINE_GUI_CONTEXT

#include <boost/asio.hpp>
#include <imgui.h>

#include "engine/gui/gui_dispatcher.h"

namespace yaga {

class IGuiContext
{
public:
  virtual ImVec2 screenSize() = 0;
  virtual GuiDispatcher& guiDispatcher() = 0;
  virtual boost::asio::thread_pool& taskPool() = 0;
};

class GuiContext
{
public:
  explicit GuiContext(IGuiContext* gui);
  ImVec2 screen() const;
  template <typename Handler>
  void render(Handler handler);
  template <typename AsyncHandler>
  void post(AsyncHandler asyncHandler);
  template <typename AsyncHandler, typename SyncHandler>
  void post(AsyncHandler asyncHandler, SyncHandler syncHandler);
private:
  IGuiContext* gui_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Handler>
void GuiContext::render(Handler handler)
{
  gui_->guiDispatcher().post(handler);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename AsyncHandler>
void GuiContext::post(AsyncHandler asyncHandler)
{
  boost::asio::post(gui_->taskPool(), asyncHandler);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename AsyncHandler, typename SyncHandler>
void GuiContext::post(AsyncHandler asyncHandler, SyncHandler syncHandler)
{
  boost::asio::post(gui_->taskPool(), [gui = gui_, asyncHandler, syncHandler]() {
    asyncHandler();
    gui->guiDispatcher().post(syncHandler);
  });
}

} // !namespace yaga

#endif // !YAGA_ENGINE_GUI_CONTEXT

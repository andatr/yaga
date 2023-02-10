#ifndef YAGA_GUI_DOCKING
#define YAGA_GUI_DOCKING

#include <memory>
#include <set>

#include "gui/context_app.h"

namespace yaga {

enum class Docking
{
  Left,
  LeftCenter,
  Right,
  RightCenter,
  Top,
  Bottom,
  Free,
  Menu,
  Status
};

class Widget;

class DockingContext
{
public:
  DockingContext(GuiContextApp* gui);
  ImVec2 getDockingPosition(Docking docking, ImVec2& size);
  void registerWidget(Widget* widget);
  void unregisterWidget(Widget* widget);
  void update();

private:
  GuiContextApp* app_;
  std::set<Widget*> widgets_;
  float top_;
  float right_;
  float bottom_;
  float left_;
  float menu_;
  float status_;
};

typedef std::shared_ptr<DockingContext> DockingContextPtr;

} // !namespace yaga

#endif // !YAGA_GUI_DOCKING

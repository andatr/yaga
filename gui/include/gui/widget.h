#ifndef YAGA_GUI_WIDGET
#define YAGA_GUI_WIDGET

#include <memory>

#include "gui/context.h"

namespace yaga {

class Widget : public std::enable_shared_from_this<Widget>
{
public:
  Widget(GuiContext context);
  virtual ~Widget();
  void docking(Docking value);
  Docking docking() const { return docking_; }
  ImVec2 size() const { return size_; }
  void enqueueRendering();
  virtual void render() = 0;

protected:
  virtual void setPosition();
  virtual void updatePosition();

private:
  bool setSizeFreeDocking();
  bool setSizeHorizontal();
  bool setSizeVertical();

protected:
  GuiContext context_;
  ImVec2 size_;
  ImVec2 position_;
  Docking docking_;
};

} // !namespace yaga

#endif // !YAGA_GUI_WIDGET

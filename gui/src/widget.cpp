#include "precompiled.h"
#include "gui/widget.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
Widget::Widget(GuiContext context) :
  context_(context),
  size_{},
  position_{},
  docking_(Docking::Free)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Widget::~Widget()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Widget::docking(Docking value)
{
  if (docking_ != Docking::Free) {
    context_.docking().unregisterWidget(this);
  }
  docking_ = value;
  if (docking_ != Docking::Free) {
    context_.docking().registerWidget(this);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Widget::enqueueRendering()
{
  context_.postSeq([w = shared_from_this()](){ w->render(); });
}

// -----------------------------------------------------------------------------------------------------------------------------
void Widget::setPosition()
{
  if (size_.x < 1.0f && size_.y < 1.0f) return;
  if (setSizeFreeDocking()) return;
  auto pos = context_.docking().getDockingPosition(docking_, size_);
  ImGui::SetNextWindowPos(pos);
  ImGui::SetNextWindowSize(size_);
  if (setSizeHorizontal()) return;
  if (setSizeVertical())   return;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Widget::updatePosition()
{
  size_     = ImGui::GetWindowSize();
  position_ = ImGui::GetWindowPos();
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Widget::setSizeFreeDocking()
{
  if (docking_ != Docking::Free) return false;
  const auto& screen = context_.screen();
  if (size_.x >= screen.x || size_.y >= screen.y) {
    size_.x = screen.x * 0.7f;
    size_.y = screen.y * 0.7f;
    position_.x = (screen.x - size_.x) / 2.0f;
    position_.y = (screen.y - size_.y) / 2.0f;
    ImGui::SetNextWindowPos(position_);
    ImGui::SetNextWindowSize(size_);
    return true;
  }
  bool update = false;
  if (position_.x < 0.0f)               { update = true; position_.x = 0.0f;               }
  if (position_.y < 0.0f)               { update = true; position_.y = 0.0f;               }
  if (position_.x + size_.x > screen.x) { update = true; position_.x = screen.x - size_.x; }
  if (position_.y + size_.y > screen.y) { update = true; position_.y = screen.y - size_.y; }
  if (update) ImGui::SetNextWindowPos(position_);
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Widget::setSizeHorizontal()
{
  if (docking_ != Docking::Left       &&
      docking_ != Docking::LeftCenter &&
      docking_ != Docking::Right      &&
      docking_ != Docking::RightCenter) return false;
  ImGui::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1));
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Widget::setSizeVertical()
{
  if (docking_ != Docking::Top && docking_ != Docking::Bottom) return false;
  ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 0), ImVec2(-1, FLT_MAX));
  return true;
}

} // !namespace yaga


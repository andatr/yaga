#include "precompiled.h"
#include "gui/docking.h"
#include "gui/widget.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
DockingContext::DockingContext(GuiContextApp* app) :
  app_(app),
  top_(0.0f),
  right_(0.0f),
  bottom_(0.0f),
  left_(0.0f),
  menu_(0.0f),
  status_(0.0f)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void DockingContext::update()
{
  menu_   = 0.0f;
  status_ = 0.0f;
  for (Widget* d : widgets_) {
    switch (d->docking()) {
    case Docking::Left:
      left_ = std::max(left_, d->size().x);
      break;
    case Docking::Right:
      right_ = std::max(right_, d->size().x);
      break;
    case Docking::Top:
      top_ = std::max(top_, d->size().x);
      break;
    case Docking::Bottom:
      bottom_ = std::max(bottom_, d->size().x);
      break;
    case Docking::Menu:
      menu_ = std::max(menu_, d->size().y);
      break;
    case Docking::Status:
      status_ = std::max(status_, d->size().y);
      break;
    }
  }
  top_ += menu_;
  bottom_ += status_;
}

// -----------------------------------------------------------------------------------------------------------------------------
ImVec2 DockingContext::getDockingPosition(Docking docking, ImVec2& size)
{
  const auto screen = app_->screenSize();
  switch (docking) {
  case Docking::Top:
    size.x = screen.x - right_ - left_;
    return ImVec2{ left_, menu_ };
  case Docking::Bottom:
    size.x = screen.x - right_ - left_;
    return ImVec2{ left_, screen.y - size.y - status_ };
  case Docking::Left:
    size.y = screen.y - menu_ - status_;
    return ImVec2{ 0.0f, menu_ };
  case Docking::Right:
    size.y = screen.y - menu_ - status_;
    return ImVec2{ screen.x - size.x, menu_ };
  case Docking::LeftCenter:
    size.y = screen.y - top_ - bottom_;
    return ImVec2{ 0.0f, menu_ + top_ };
  case Docking::RightCenter:
    size.y = screen.y - top_ - bottom_;
    return ImVec2{ screen.x - size.x, menu_ + top_ };
  default:
    break;
  }
  return ImVec2{};
}

// -----------------------------------------------------------------------------------------------------------------------------
void DockingContext::registerWidget(Widget* widget)
{
  widgets_.insert(widget);
}

// -----------------------------------------------------------------------------------------------------------------------------
void DockingContext::unregisterWidget(Widget* widget)
{
  widgets_.erase(widget);
}

} // !namespace yaga


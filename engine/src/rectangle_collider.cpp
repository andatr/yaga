#include "precompiled.h"
#include "engine/rectangle_collider.h"

namespace yaga {
namespace {

const std::string componentName = "RectangleCollider";

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
RectangleCollider::RectangleCollider(const glm::vec2& leftTop, const glm::vec2& rightBottom) :
  leftTop_(leftTop),
  rightBottom_(rightBottom)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
RectangleCollider::~RectangleCollider()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
const std::string& RectangleCollider::name()
{
  return componentName;
}

// -----------------------------------------------------------------------------------------------------------------------------
bool RectangleCollider::collide(const glm::vec2& point)
{
  return point.x >= leftTop_.x && point.x <= rightBottom_.x &&
         point.y >= leftTop_.y && point.y <= rightBottom_.y;
}

} // !namespace yaga

#include "precompiled.h"
#include "engine/circle_collider.h"

namespace yaga {
namespace {

const std::string componentName = "CircleCollider";

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
CircleCollider::CircleCollider(const glm::vec2& center, float radius) :
  center_(center),
  radius_(radius * radius)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
CircleCollider::~CircleCollider()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
const std::string& CircleCollider::name()
{
  return componentName;
}

// -----------------------------------------------------------------------------------------------------------------------------
bool CircleCollider::collide(const glm::vec2& point)
{
  const float w = point.x - center_.x;
  const float h = point.y - center_.y;
  return w * w + h * h <= radius_;
}

} // !namespace yaga

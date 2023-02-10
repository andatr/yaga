#ifndef YAGA_ENGINE_RECTANGLE_COLLIDER
#define YAGA_ENGINE_RECTANGLE_COLLIDER

#include "engine/collider.h"

namespace yaga {

class RectangleCollider : public Collider
{
public:
  RectangleCollider(const glm::vec2& leftTop, const glm::vec2& rightBottom);
  virtual ~RectangleCollider();
  bool collide(const glm::vec2& point) override;
  const std::string& name() override;

private:
  glm::vec2 leftTop_;
  glm::vec2 rightBottom_;
};

} // !namespace yaga

#endif // !YAGA_ENGINE_RECTANGLE_COLLIDER

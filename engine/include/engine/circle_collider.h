#ifndef YAGA_ENGINE_CIRCLE_COLLIDER
#define YAGA_ENGINE_CIRCLE_COLLIDER

#include "engine/collider.h"

namespace yaga {

class CircleCollider : public Collider
{
public:
  CircleCollider(const glm::vec2& center, float radius);
  virtual ~CircleCollider();
  bool collide(const glm::vec2& point) override;
  const std::string& name() override;

private:
  glm::vec2 center_;
  float radius_;
};

} // !namespace yaga

#endif // !YAGA_ENGINE_CIRCLE_COLLIDER

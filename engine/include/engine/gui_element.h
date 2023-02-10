#ifndef YAGA_ENGINE_COLLIDER
#define YAGA_ENGINE_COLLIDER

#include <memory>

#include "component.h"
#include "utility/glm.h"

namespace yaga {

class Collider : public Component
{
public:
  virtual ~Collider() {}
  virtual bool collide(const glm::vec2& point) = 0;
};

typedef std::unique_ptr<Collider> ColliderPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_COLLIDER

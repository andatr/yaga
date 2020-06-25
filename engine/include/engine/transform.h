#ifndef YAGA_ENGINE_TRANSFORM
#define YAGA_ENGINE_TRANSFORM

#include <memory>
#include <unordered_set>

#include "engine/component.h"
#include "engine/glm.h"

namespace yaga
{

class Transform : public Component
{
public:
  static const uint32_t worldProperty = 1;
  typedef std::unordered_set<Transform*> TransformSet;
public:
  explicit Transform(Object* obj);
  virtual ~Transform();
  const glm::mat4& world() const { return world_; }
  const glm::mat4& local() const { return local_; }
  virtual void local(const glm::mat4& m);
  Transform* parent() const { return parent_; }
  virtual void parent(Transform* parent);
  const TransformSet& children() const { return children_; }
protected:
  void updateWorld();
protected:
  glm::mat4 world_;
  glm::mat4 local_;
  Transform* parent_;
  TransformSet children_;
  Connection parentConnection_;
};

typedef std::unique_ptr<Transform> TransformPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_TRANSFORM

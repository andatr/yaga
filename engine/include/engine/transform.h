#ifndef YAGA_ENGINE_TRANSFORM
#define YAGA_ENGINE_TRANSFORM

#include <memory>
#include <unordered_set>

#include "assets/transform.h"
#include "engine/component.h"
#include "utility/glm.h"
#include "utility/property.h"

namespace yaga {

class Transform : public Component
{
friend class Context;

public:
  struct PropertyIndex
  {
    static const int local  = 0;
    static const int world  = 1;
    static const int parent = 2;
  };
  typedef std::unordered_set<Transform*> TransformSet;

public:
  virtual ~Transform();
  const glm::mat4& world() const { return world_; }
  const glm::mat4& local() const { return local_; }
  Transform* parent()      const { return parent_; }
  const TransformSet& children() const { return children_; }
  virtual void parent(Transform* parent);
  const std::string& name() override;
  assets::TransformPtr asset() { return asset_; }

protected:
  explicit Transform(assets::TransformPtr asset);
  void updateLocal();
  void updateWorld();

protected:
  assets::TransformPtr asset_;
  glm::mat4 world_;
  glm::mat4 local_;
  Transform* parent_;
  TransformSet children_;
  SignalConnections connections_;
};

typedef std::unique_ptr<Transform> TransformPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_TRANSFORM

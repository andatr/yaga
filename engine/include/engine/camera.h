#ifndef YAGA_ENGINE_CAMERA
#define YAGA_ENGINE_CAMERA

#include <memory>
#include <string>

#include "assets/camera.h"
#include "engine/component.h"
#include "engine/transform.h"
#include "utility/signal.h"

namespace yaga {

class Camera : public Component
{
friend class Context;

public:
  struct PropertyIndex
  {
    static const int projection = 0;
    static const int view       = 1;
  };

public:
  virtual ~Camera();
  const glm::mat4&   view()       const { return view_;       }
  const glm::mat4&   projection() const { return projection_; }
  const std::string& name() override;
  assets::CameraPtr asset() { return asset_; }
  virtual Camera* projection(const glm::mat4& value);

protected:
  explicit Camera(assets::CameraPtr asset);
  void onAttached(Object* object) override;
  void updateView();

private:
  void onComponentAdd(Component* component) override;
  void onComponentRemove(Component* component) override;
  virtual void onTransformUpdate(void*);

protected:
  assets::CameraPtr asset_;
  glm::mat4 projection_;
  glm::mat4 view_;
  Transform* transform_;
  SignalConnections connections_;
};

typedef std::unique_ptr<Camera> CameraPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_CAMERA

#ifndef YAGA_ENGINE_CAMERA
#define YAGA_ENGINE_CAMERA

#include <memory>

#include "engine/component.h"
#include "engine/transform.h"

namespace yaga {

class Camera : public Component
{
public:
  static const uint32_t viewProperty = 1;

public:
  explicit Camera(Object* obj);
  virtual ~Camera();
  const glm::mat4& view() const { return view_; }
  const glm::mat4& projection() const { return projection_; }
  virtual void projection(const glm::mat4& value);
  const glm::vec3& lookAt() const { return lookAt_; }
  virtual void lookAt(const glm::vec3& lookAt);

protected:
  virtual void updateView();

private:
  void onComponentAdd(Component* component) override;
  void onComponentRemove(Component* component) override;
  virtual void onTransformUpdated(uint32_t prop);

protected:
  Transform* transform_;
  glm::mat4  projection_;
  glm::vec3  lookAt_;
  glm::mat4  view_;

private:
  Connection transformConnection_;
};

typedef std::unique_ptr<Camera> CameraPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_CAMERA

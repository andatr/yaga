#ifndef YAGA_ENGINE_CAMERA
#define YAGA_ENGINE_CAMERA

#include <memory>

#include "transform.h"
#include "engine/component.h"
#include "engine/asset/camera.h"

namespace yaga
{

class Camera : public Component
{
public:
  static const uint32_t viewProperty = 1;
public:
  explicit Camera(Object* obj, asset::Camera* asset);
  virtual ~Camera();
protected:
  virtual void updateView();
private:
  void onComponentAdd(Component* component) override;
  void onComponenRemove(Component* component) override;
  virtual void onTransformUpdated(uint32_t prop);
  virtual void onAssetUpdated(asset::CameraProperty prop);
protected:
  asset::Camera* asset_;
  glm::mat4 view_;
  Transform* transform_;
private:
  Connection assetConnection_;
  Connection transformConnection_;
};

typedef std::unique_ptr<Camera> CameraPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_CAMERA

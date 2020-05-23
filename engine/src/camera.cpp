#include "precompiled.h"
#include "camera.h"
#include "object.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Camera::Camera(Object* obj, asset::Camera* asset) :
  Component(obj), asset_(asset), transform_(obj->getComponent<Transform>()), view_{}
{
  assetConnection_ = asset_->onUpdate(std::bind(&Camera::onAssetUpdated, this, std::placeholders::_1));
  if (transform_) {
    transformConnection_ = transform_->onUpdate(std::bind(&Camera::onTransformUpdated, this, std::placeholders::_1));
    updateView();
  }
}

// -------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
  asset_->onUpdate(assetConnection_);
  if (transform_) {
    transform_->onUpdate(transformConnection_);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Camera::onAssetUpdated(asset::CameraProperty prop)
{
  if (prop == asset::CameraProperty::lookAt) {
    updateView();
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Camera::onTransformUpdated(uint32_t prop)
{
  if (prop == Transform::worldProperty) {
    updateView();
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Camera::updateView()
{
  const auto& matrix = transform_->world();
  const auto position = glm::vec3(matrix[3]);
  const auto up = glm::vec3(0.0f, 1.0f, 0.0f);
  view_ = glm::lookAt(position, asset_->lookAt(), up);
  fireUpdate(viewProperty);
}

// -------------------------------------------------------------------------------------------------------------------------
void Camera::onComponentAdd(Component* component)
{
  if (transform_) return;
  transform_ = dynamic_cast<Transform*>(component);
  if (transform_) {
    transformConnection_ = transform_->onUpdate(std::bind(&Camera::onTransformUpdated, this, std::placeholders::_1));
    updateView();
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Camera::onComponenRemove(Component* component)
{
  if (!transform_) return;
  if (component == transform_) {
    transform_->onUpdate(transformConnection_);
    transform_ = nullptr;
  }
}

} // !namespace yaga


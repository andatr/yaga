#include "precompiled.h"
#include "camera.h"
#include "object.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
Camera::Camera(Object* obj) :
  Component(obj),
  transform_(obj->getComponent<Transform>()),
  view_{}
{
  if (transform_) {
    transformConnection_ = transform_->onUpdate(std::bind(&Camera::onTransformUpdated, this, std::placeholders::_1));
    updateView();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
  if (transform_) {
    transform_->onUpdate(transformConnection_);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::onTransformUpdated(uint32_t prop)
{
  if (prop == Transform::worldProperty) {
    updateView();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::lookAt(const glm::vec3& lookAt)
{
  lookAt_ = lookAt;
  updateView();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::projection(const glm::mat4& value)
{
  projection_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::updateView()
{
  const auto& matrix  = transform_->world();
  const auto position = glm::vec3(matrix[3]);
  const auto up       = glm::vec3(0.0f, 1.0f, 0.0f);
  view_ = glm::lookAt(position, lookAt_, up);
  fireUpdate(viewProperty);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::onComponentAdd(Component* component)
{
  if (transform_) return;
  transform_ = dynamic_cast<Transform*>(component);
  if (transform_) {
    transformConnection_ = transform_->onUpdate(std::bind(&Camera::onTransformUpdated, this, std::placeholders::_1));
    updateView();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::onComponentRemove(Component* component)
{
  if (!transform_) return;
  if (component == transform_) {
    transform_->onUpdate(transformConnection_);
    transform_ = nullptr;
  }
}

} // !namespace yaga

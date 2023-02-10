#include "precompiled.h"
#include "engine/camera.h"
#include "engine/object.h"

namespace yaga {
namespace {

const std::string componentName = "Camera";
const int transformConnection = 0;

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Camera::Camera(assets::CameraPtr asset) :
  asset_(asset),
  projection_{},
  view_{},
  transform_{}
{
  connections_.push_back(nullptr); // transform connection
  addProperty("Projection", &projection_);
  addProperty("View",       &view_      );
  auto handler = [this](void*) { updateView(); };
  connections_.push_back(asset->properties(assets::Camera::PropertyIndex::lookAt)->onUpdate(handler));
  handler(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::onAttached(Object* object)
{
  onComponentAdd(object->getComponent<Transform>());
}

// -----------------------------------------------------------------------------------------------------------------------------
Camera* Camera::projection(const glm::mat4& value)
{
  projection_ = value;
  properties_[PropertyIndex::projection]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::updateView()
{
  if (!transform_) return;
  const auto& matrix  = transform_->world();
  const auto position = glm::vec3(matrix[3]);
  const auto up       = glm::vec3(0.0f, 1.0f, 0.0f);
  view_ = glm::lookAt(position, asset_->lookAt(), up);
  properties_[PropertyIndex::view]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::onComponentAdd(Component* component)
{
  if (transform_) return;
  transform_ = dynamic_cast<Transform*>(component);
  if (transform_) {
    connections_[transformConnection] =
      transform_->properties(Transform::PropertyIndex::world)->onUpdate(
        [this](void* sender) { onTransformUpdate(sender); });
    updateView();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::onComponentRemove(Component* component)
{
  if (!transform_) return;
  if (component == transform_) {
    connections_[transformConnection] = nullptr;
    transform_ = nullptr;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::onTransformUpdate(void*)
{
  updateView();
}

// -----------------------------------------------------------------------------------------------------------------------------
const std::string& Camera::name()
{
  return componentName;
}

} // !namespace yaga

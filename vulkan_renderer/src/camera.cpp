#include "precompiled.h"
#include "camera.h"
#include "camera_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Camera::Camera(Object* obj, assets::Camera* asset, CameraPool* pool, uint32_t frames) :
  yaga::Camera(obj, asset), pool_(pool), frames_(frames)
{
  for (size_t i = 0; i < frames_.size(); ++i) {
    frames_[i].dirty = true;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
  pool_->removeCamera(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::updateView()
{
  base::updateView();
  for (size_t i = 0; i < frames_.size(); ++i) {
    frames_[i].dirty = true;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::onAssetUpdated(assets::CameraProperty prop)
{
  if (prop == assets::CameraProperty::projection) {
    for (size_t i = 0; i < frames_.size(); ++i) {
      frames_[i].dirty = true;
    }
  }
}

} // !namespace vk
} // !namespace yaga
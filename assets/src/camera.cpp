#include "precompiled.h"
#include "assets/camera.h"

namespace yaga
{
namespace assets
{

const SerializationInfo Camera::serializationInfo = {
  (uint32_t)StandardAssetId::camera,
  { "ycam" },
  &Camera::deserializeBinary,
  &Camera::deserializeFriendly
};

// -------------------------------------------------------------------------------------------------------------------------
Camera::Camera(const std::string& name) :
  Asset(name), projection_{}, lookAt_(1.0f, 0.0f, 0.0f)
{
  projection_ = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  projection_[1][1] *= -1;
}
  
// -------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Camera::projection(const glm::mat4& p)
{
  projection_ = p;
  fireUpdate(CameraProperty::projection);
}

// -------------------------------------------------------------------------------------------------------------------------
void Camera::lookAt(const glm::vec3& lookAt)
{
  lookAt_ = lookAt;
  fireUpdate(CameraProperty::lookAt);
}

// -------------------------------------------------------------------------------------------------------------------------
CameraPtr Camera::deserializeBinary(const std::string&, std::istream&, size_t, RefResolver&)
{
  THROW_NOT_IMPLEMENTED;
}

// -------------------------------------------------------------------------------------------------------------------------
CameraPtr Camera::deserializeFriendly(const std::string& name, const std::string&, RefResolver&)
{
  auto camera = std::make_unique<Camera>(name);
  return camera;
}

} // !namespace assets
} // !namespace yaga

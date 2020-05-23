#include "precompiled.h"
#include "asset/camera.h"

namespace yaga
{
namespace asset
{

const SerializationInfo Camera::serializationInfo = {
  9,
  { "ycam" },
  "Camera",
  &Camera::deserialize,
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
CameraPtr Camera::deserialize(const std::string& name, std::istream& stream, size_t size)
{
  return deserializeFriendly("", name, stream, size);
}

// -------------------------------------------------------------------------------------------------------------------------
CameraPtr Camera::deserializeFriendly(const std::string&, const std::string& name, std::istream&, size_t)
{
  auto camera = std::make_unique<Camera>(name);
  return camera;
}

} // !namespace asset
} // !namespace yaga

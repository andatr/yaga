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
  Asset(name)
{
}
  
// -------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
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

#include "precompiled.h"
#include "assets/camera.h"
#include "binary_serializer_helper.h"
#include "binary_serializer_registry.h"
#include "friendly_serializer_helper.h"

namespace yaga {
namespace assets {
namespace {

constexpr const char* LOOK_AT_PNAME = "lookAt";

} // !namespace

BINARY_SERIALIZER_REG(Camera)

// -----------------------------------------------------------------------------------------------------------------------------
Camera::Camera(const std::string& name) :
  Asset(name),
  lookAt_(1.0f, 0.0f, 0.0f)
{
  addProperty("LookAt", &lookAt_);
}

// -----------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Camera* Camera::lookAt(const glm::vec3& value)
{
  lookAt_ = value;
  properties_[PropertyIndex::lookAt]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
CameraPtr Camera::deserializeBinary(std::istream& stream)
{
  std::string name;
  binser::read(stream, name);
  auto camera = std::make_unique<Camera>(name);
  binser::read(stream, camera->lookAt_);
  return camera;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::serializeBinary(Asset* asset, std::ostream& stream)
{
  auto camera = dynamic_cast<Camera*>(asset);
  if (!camera) THROW("Camera serializer got wrong asset");
  binser::write(stream, camera->name_);
  binser::write(stream, camera->lookAt_);
}

// -----------------------------------------------------------------------------------------------------------------------------
CameraPtr Camera::deserializeFriendly(std::istream& stream)
{
  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_json(stream, props);
  auto camera = frser::createAsset<Camera>(props);
  frser::read(props, LOOK_AT_PNAME, camera->lookAt_);
  return camera;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::serializeFriendly(Asset* asset, std::ostream& stream)
{
  namespace pt = boost::property_tree;
  auto camera = dynamic_cast<Camera*>(asset);
  if (!camera) THROW("Camera serializer got wrong asset");
  pt::ptree props;
  frser::write(props, frser::NAME_PNAME, camera->name_  );
  frser::write(props, LOOK_AT_PNAME,     camera->lookAt_);
  pt::write_json(stream, props);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Camera::resolveRefs(Asset*, Storage*)
{
}

} // !namespace assets
} // !namespace yaga

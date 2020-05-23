#ifndef YAGA_ENGINE_ASSET_CAMERA
#define YAGA_ENGINE_ASSET_CAMERA

#include <memory>
#include <istream>
#include <ostream>
#include <glm/glm.hpp>

#include "engine/asset/asset.h"
#include "engine/asset/serializer.h"
#include "utility/update_notifier.h"

namespace yaga
{
namespace asset
{

class Camera;
typedef std::unique_ptr<Camera> CameraPtr;

enum class CameraProperty
{
  projection,
  lookAt
};

class Camera : public Asset, public UpdateNotifier<CameraProperty>
{
public:
  explicit Camera(const std::string& name);
  virtual ~Camera();
  const glm::mat4& projection() const { return projection_; }
  virtual void projection(const glm::mat4& p);
  const glm::vec3& lookAt() const { return lookAt_; }
  void lookAt(const glm::vec3& lookAt);
public:
  static const SerializationInfo serializationInfo;
  static CameraPtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static CameraPtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
private:
  glm::mat4 projection_;
  glm::vec3 lookAt_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_CAMERA

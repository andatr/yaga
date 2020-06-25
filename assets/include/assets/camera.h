#ifndef YAGA_ASSETS_CAMERA
#define YAGA_ASSETS_CAMERA

#include <memory>
#include <istream>
#include <ostream>

#include "assets/glm.h"
#include "assets/asset.h"
#include "assets/serializer.h"
#include "utility/update_notifier.h"

namespace yaga
{
namespace assets
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
  static CameraPtr deserializeBinary(const std::string& name, std::istream& stream,
    size_t size, RefResolver& resolver);
  static CameraPtr deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver);
private:
  glm::mat4 projection_;
  glm::vec3 lookAt_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_CAMERA

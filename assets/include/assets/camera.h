#ifndef YAGA_ASSETS_CAMERA
#define YAGA_ASSETS_CAMERA

#include <istream>
#include <memory>
#include <ostream>

#include "assets/asset.h"
#include "assets/serializer.h"
#include "utility/glm.h"

namespace yaga {
namespace assets {

class Camera;
typedef std::shared_ptr<Camera> CameraPtr;

class Camera : public Asset
{
public:
  struct PropertyIndex
  {
    static const int lookAt = 0;
  };

public:
  explicit Camera(const std::string& name);
  virtual ~Camera();
  const glm::vec3& lookAt() const { return lookAt_; }
  Camera* lookAt(const glm::vec3& value);
  AssetType type() const override { return typeId; }

public:
  static const AssetType typeId = (uint32_t)StandardAssetType::camera;
  static CameraPtr deserializeBinary  (std::istream& stream);
  static CameraPtr deserializeFriendly(std::istream& stream);
  static void serializeBinary  (Asset* asset, std::ostream& stream);
  static void serializeFriendly(Asset* asset, std::ostream& stream);
  static void resolveRefs(Asset* asset, Storage* storage);

private:
  glm::vec3 lookAt_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_CAMERA

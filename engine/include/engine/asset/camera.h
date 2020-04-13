#ifndef YAGA_ENGINE_ASSET_CAMERA
#define YAGA_ENGINE_ASSET_CAMERA

#include <memory>
#include <istream>
#include <ostream>

#include "engine/asset/asset.h"
#include "engine/asset/serializer.h"

namespace yaga
{
namespace asset
{

class Camera;
typedef std::unique_ptr<Camera> CameraPtr;

class Camera : public Asset
{
public:
  explicit Camera(const std::string& name);
  virtual ~Camera();
public:
  static const SerializationInfo serializationInfo;
  static CameraPtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static CameraPtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
private:
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_CAMERA

#ifndef YAGA_ASSETS_TRANSFORM
#define YAGA_ASSETS_TRANSFORM

#include <istream>
#include <memory>
#include <ostream>

#include "assets/asset.h"
#include "assets/serializer.h"
#include "utility/glm.h"

namespace yaga {
namespace assets {

class Transform;
typedef std::shared_ptr<Transform> TransformPtr;

class Transform : public Asset
{
public:
  struct PropertyIndex
  {
    static const int translation = 0;
    static const int rotation    = 1;
    static const int scale       = 2;
    static const int parent      = 3;
  };

public:
  explicit Transform(const std::string& name);
  virtual ~Transform();
  const glm::vec3&   translation() const { return translation_; }
  const glm::quat&   rotation()    const { return rotation_;    }
  const glm::vec3&   scale()       const { return scale_;       }
  TransformPtr       parent()      const { return parent_;      }
  Transform* translation(const glm::vec3&   value);
  Transform* rotation   (const glm::quat&   value);
  Transform* scale      (const glm::vec3&   value);
  Transform* parent     (TransformPtr       value);
  AssetType type() const override { return typeId; }

public:
  static const AssetType typeId = (uint32_t)StandardAssetType::transform;
  static TransformPtr deserializeBinary  (std::istream& stream);
  static TransformPtr deserializeFriendly(std::istream& stream);
  static void serializeBinary  (Asset* asset, std::ostream& stream);
  static void serializeFriendly(Asset* asset, std::ostream& stream);
  static void resolveRefs(Asset* asset, Storage* storage);

private:
  glm::vec3 translation_;
  glm::quat rotation_;
  glm::vec3 scale_;
  std::string parentName_;
  TransformPtr parent_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_TRANSFORM

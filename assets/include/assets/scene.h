#ifndef YAGA_ASSETS_SCENE
#define YAGA_ASSETS_SCENE

#include <istream>
#include <memory>

#include "assets/asset.h"
#include "assets/model.h"
#include "assets/serializer.h"

namespace yaga {
namespace assets {

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class Scene : public Asset
{
public:
  explicit Scene(const std::string& name);
  virtual ~Scene();
  Model* model() const { return model_; }
  AssetType type() const override { return typeId; }

public:
  static const AssetType typeId = (uint32_t)StandardAssetType::scene;
  static ScenePtr deserializeBinary  (std::istream& stream);
  static ScenePtr deserializeFriendly(std::istream& stream);
  static void serializeBinary  (Asset* asset, std::ostream& stream);
  static void serializeFriendly(Asset* asset, std::ostream& stream);
  static void resolveRefs(Asset* asset, Storage* storage);

private:
  Model* model_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_SCENE

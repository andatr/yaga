#ifndef YAGA_ASSETS_MODEL
#define YAGA_ASSETS_MODEL

#include <istream>
#include <memory>

#include "assets/asset.h"
#include "assets/material.h"
#include "assets/mesh.h"
#include "assets/serializer.h"

namespace yaga {
namespace assets {

class Model;
typedef std::shared_ptr<Model> ModelPtr;

class Model : public Asset
{
public:
  explicit Model(const std::string& name);
  virtual ~Model();
  MaterialPtr material() const { return material_; }
  MeshPtr         mesh() const { return mesh_;     }
  AssetType type() const override { return typeId; }

public:
  static const AssetType typeId = (uint32_t)StandardAssetType::model;
  static ModelPtr deserializeBinary  (std::istream& stream);
  static ModelPtr deserializeFriendly(std::istream& stream);
  static void serializeBinary  (Asset* asset, std::ostream& stream);
  static void serializeFriendly(Asset* asset, std::ostream& stream);
  static void resolveRefs(Asset* asset, Storage* storage);

private:
  std::string materialName_;
  std::string meshName_;
  MaterialPtr material_;
  MeshPtr mesh_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_MODEL

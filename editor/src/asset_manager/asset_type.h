#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE

#include <memory>
#include <string>

#include "assets/asset.h"

namespace yaga {
namespace editor {

typedef yaga::assets::AssetType AssetTypeId;

class AssetType
{
public:
  explicit AssetType(AssetTypeId id, const std::string& name, const std::string& extension, bool external);
  explicit AssetType(assets::StandardAssetType id, const std::string& name, const std::string& extension, bool external);
  virtual ~AssetType() {}
  AssetTypeId id() const { return id_; }
  const std::string name() const { return name_; }
  const std::string extension() const { return extension_; }
  bool external() const { return external_; }
  virtual assets::AssetPtr createAsset(const std::string& name, const std::string& importFilename) = 0;

protected:
  AssetTypeId id_;
  std::string name_;
  std::string extension_;
  bool external_;
};

typedef std::unique_ptr<AssetType> AssetTypePtr;

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE

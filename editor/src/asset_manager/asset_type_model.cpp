#include "precompiled.h"
#include "asset_type_model.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeModel::AssetTypeModel() :
  AssetType(assets::StandardAssetType::model, "model", "ymod", true)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeModel::createAsset(const std::string& name, const std::string& filename)
{
  return nullptr;
}

} // !namespace editor
} // !namespace yaga

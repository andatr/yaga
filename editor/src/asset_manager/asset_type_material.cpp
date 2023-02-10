#include "precompiled.h"
#include "asset_type_material.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeMaterial::AssetTypeMaterial() :
  AssetType(assets::StandardAssetType::material, "material", "ymat", false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeMaterial::createAsset(const std::string& name, const std::string& filename)
{
  return nullptr;
}

} // !namespace editor
} // !namespace yaga

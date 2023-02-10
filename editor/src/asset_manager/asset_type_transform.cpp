#include "precompiled.h"
#include "asset_type_transform.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeTransform::AssetTypeTransform() :
  AssetType(assets::StandardAssetType::transform, "text", "ytfm", false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeTransform::createAsset(const std::string& name, const std::string& filename)
{
  return nullptr;
}

} // !namespace editor
} // !namespace yaga

#include "precompiled.h"
#include "asset_type_text.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeText::AssetTypeText() :
  AssetType(assets::StandardAssetType::text, "text", "ytxt", false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeText::createAsset(const std::string& name, const std::string& filename)
{
  return nullptr;
}

} // !namespace editor
} // !namespace yaga

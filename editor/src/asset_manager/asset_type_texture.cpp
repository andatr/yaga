#include "precompiled.h"
#include "asset_type_texture.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeTexture::AssetTypeTexture() :
  AssetType(assets::StandardAssetType::texture, "texture ", "ytxr", false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeTexture::createAsset(const std::string& name, const std::string& filename)
{
  return nullptr;
}

} // !namespace editor
} // !namespace yaga

#include "precompiled.h"
#include "asset_type_scene.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeScene::AssetTypeScene() :
  AssetType(assets::StandardAssetType::scene, "scene", "yscn", false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeScene::createAsset(const std::string& name, const std::string& filename)
{
  return nullptr;
}

} // !namespace editor
} // !namespace yaga

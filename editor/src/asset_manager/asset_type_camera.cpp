#include "precompiled.h"
#include "asset_type_camera.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeCamera::AssetTypeCamera() :
  AssetType(assets::StandardAssetType::camera, "camera", "ycam", false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeCamera::createAsset(const std::string& name, const std::string& filename)
{
  return nullptr;
}

} // !namespace editor
} // !namespace yaga

#include "precompiled.h"
#include "asset_type_mesh.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeMesh::AssetTypeMesh() :
  AssetType(assets::StandardAssetType::mesh, "mesh", "ymsh", true)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeMesh::createAsset(const std::string&, const std::string&)
{
  return nullptr;
}

} // !namespace editor
} // !namespace yaga

#include "precompiled.h"
#include "asset_type_shader.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeShader::AssetTypeShader() :
  AssetType(assets::StandardAssetType::shader, "shader", "ysdr", true)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeShader::createAsset(const std::string& name, const std::string& filename)
{
  return nullptr;
}

} // !namespace editor
} // !namespace yaga

#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_SHADER
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_SHADER

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeShader : public AssetType
{
public:
  explicit AssetTypeShader();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_SHADER

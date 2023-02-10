#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_TEXTURE
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_TEXTURE

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeTexture : public AssetType
{
public:
  explicit AssetTypeTexture();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_TEXTURE

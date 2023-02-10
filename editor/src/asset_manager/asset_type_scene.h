#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_SCENE
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_SCENE

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeScene : public AssetType
{
public:
  explicit AssetTypeScene();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_SCENE

#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_CAMERA
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_CAMERA

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeCamera : public AssetType
{
public:
  explicit AssetTypeCamera();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_CAMERA

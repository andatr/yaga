#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_IMAGE
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_IMAGE

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeImage : public AssetType
{
public:
  explicit AssetTypeImage();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_IMAGE

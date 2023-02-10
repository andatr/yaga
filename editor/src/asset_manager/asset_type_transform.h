#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_TRANSFORM
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_TRANSFORM

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeTransform : public AssetType
{
public:
  explicit AssetTypeTransform();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_TRANSFORM

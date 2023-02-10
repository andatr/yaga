#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_MATERIAL
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_MATERIAL

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeMaterial : public AssetType
{
public:
  explicit AssetTypeMaterial();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_MATERIAL

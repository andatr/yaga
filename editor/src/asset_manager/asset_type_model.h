#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_MODEL
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_MODEL

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeModel : public AssetType
{
public:
  explicit AssetTypeModel();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_MODEL

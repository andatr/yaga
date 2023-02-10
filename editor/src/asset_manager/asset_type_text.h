#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_TEXT
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_TEXT

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeText : public AssetType
{
public:
  explicit AssetTypeText();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_TEXT

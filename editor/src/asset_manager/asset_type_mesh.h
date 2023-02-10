#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_MESH
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_MESH

#include "asset_type.h"

namespace yaga {
namespace editor {

class AssetTypeMesh : public AssetType
{
public:
  explicit AssetTypeMesh();

protected:
  assets::AssetPtr createAsset(const std::string& name, const std::string& filename) override;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_TYPE_MESH

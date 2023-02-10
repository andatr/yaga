#include "precompiled.h"
#include "asset_type.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetType::AssetType(AssetTypeId id, const std::string& name, const std::string& extension, bool external) :
  id_(id),
  name_(name),
  extension_(extension),
  external_(external)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetType::AssetType(assets::StandardAssetType id, const std::string& name, const std::string& extension, bool external) :
  AssetType(static_cast<AssetTypeId>(id), name, extension, external)
{
}

} // !namespace editor
} // !namespace yaga

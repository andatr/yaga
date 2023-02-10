#include "precompiled.h"
#include "asset_registry.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetRegistry& AssetRegistry::instance()
{
  static AssetRegistry inst;
  return inst;
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetRegistry::add(const char* name, assets::AssetType type)
{
  storage_[type] = { name };
}

} // !namespace editor
} // !namespace yaga

#include "precompiled.h"
#include "database.h"

namespace yaga
{
namespace asset
{

// -------------------------------------------------------------------------------------------------------------------------
Asset* Database::Get(const std::string& name)
{
  auto it = assets_.find(name);
  if (it == assets_.end()) {
    THROW("Could not find asset \"%1%\"", name);
  }
  return it->second.get();
}

// -------------------------------------------------------------------------------------------------------------------------
void Database::Put(AssetPtr asset, bool resolveRefs)
{
  // auto it = assets_.find(asset->Name());
  // if (it != assets_.end())
  //  THROW("Asset with name \"%1%\" already exists", asset->Name());
  if (resolveRefs) {
    asset->ResolveRefs(this);
  }
  assets_[asset->Name()] = std::move(asset);
}

// -------------------------------------------------------------------------------------------------------------------------
void Database::ResolveRefs()
{
  for (const auto& asset: assets_) {
    asset.second->ResolveRefs(this);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Database::Delete(AssetPtr asset)
{
  assets_.erase(asset->Name());
}

// -------------------------------------------------------------------------------------------------------------------------
void Database::Delete(const std::string& name)
{
  assets_.erase(name);
}

} // !namespace asset
} // !namespace yaga


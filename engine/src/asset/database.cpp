#include "precompiled.h"
#include "asset/database.h"

namespace yaga
{
namespace asset
{

// -------------------------------------------------------------------------------------------------------------------------
Asset* Database::get(const std::string& name) const
{
  auto it = assets_.find(name);
  if (it == assets_.end()) {
    THROW("Could not find asset \"%1%\"", name);
  }
  return it->second.get();
}

// -------------------------------------------------------------------------------------------------------------------------
void Database::put(AssetPtr asset, bool resolveRefs)
{
  // auto it = assets_.find(asset->Name());
  // if (it != assets_.end())
  //  THROW("Asset with name \"%1%\" already exists", asset->Name());
  if (resolveRefs) {
    asset->resolveRefs(this);
  }
  assets_[asset->name()] = std::move(asset);
}

// -------------------------------------------------------------------------------------------------------------------------
void Database::resolveRefs()
{
  for (const auto& asset: assets_) {
    asset.second->resolveRefs(this);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Database::remove(Asset* asset)
{
  assets_.erase(asset->name());
}

// -------------------------------------------------------------------------------------------------------------------------
void Database::remove(const std::string& name)
{
  assets_.erase(name);
}

} // !namespace asset
} // !namespace yaga


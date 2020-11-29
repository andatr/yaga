#include "precompiled.h"
#include "assets/storage.h"

namespace yaga {
namespace assets {

// -----------------------------------------------------------------------------------------------------------------------------
Asset* Storage::get(const std::string& name) const
{
  auto it = assets_.find(name);
  if (it == assets_.end()) {
    THROW("Could not find asset \"%1%\"", name);
  }
  return it->second.get();
}

// -----------------------------------------------------------------------------------------------------------------------------
Asset* Storage::tryGet(const std::string& name) const
{
  auto it = assets_.find(name);
  if (it == assets_.end()) {
    return nullptr;
  }
  return it->second.get();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Storage::put(AssetPtr asset)
{
  auto it = assets_.find(asset->name());
  if (it != assets_.end()) {
    THROW("Asset with name \"%1%\" already exists", asset->name());
  }
  assets_[asset->name()] = std::move(asset);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Storage::remove(Asset* asset)
{
  assets_.erase(asset->name());
}

// -----------------------------------------------------------------------------------------------------------------------------
void Storage::remove(const std::string& name)
{
  assets_.erase(name);
}

} // !namespace assets
} // !namespace yaga

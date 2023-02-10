#include "precompiled.h"
#include "assets/storage.h"

namespace yaga {
namespace assets {

/*
// -----------------------------------------------------------------------------------------------------------------------------
Asset* Storage::get(const std::string& name)
{
  auto asset = tryGet(name);
  if (!asset) {
    THROW("Could not find asset \"%1%\"", name);
  }
  return asset;
}

// -----------------------------------------------------------------------------------------------------------------------------
Asset* Storage::tryGet(const std::string& name)
{
  auto it = assets_.find(name);
  if (it == assets_.end()) {
    return nullptr;
  }
  return it->second.get();
}

// -----------------------------------------------------------------------------------------------------------------------------
Asset* Storage::put(AssetPtr asset)
{
  auto it = assets_.find(asset->name());
  if (it != assets_.end()) {
    THROW("Asset with name \"%1%\" already exists", asset->name());
  }
  auto ptr = asset.get();
  assets_[asset->name()] = std::move(asset);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetPtr Storage::remove(Asset* asset)
{
  return remove(asset->name());
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetPtr Storage::remove(const std::string& name)
{
  auto it = assets_.find(name);
  if (it == assets_.end()) return nullptr;
  auto asset = std::move(it->second);
  assets_.erase(it);
  return asset;
}
*/

} // !namespace assets
} // !namespace yaga

#include "precompiled.h"
#include "asset_manager.h"

namespace fs = boost::filesystem;

namespace yaga {
namespace editor {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
inline std::string getAssetName(const fs::path& root, const fs::path& path)
{
  auto name = path.lexically_relative(root).string();
  std::replace(name.begin(), name.end(), '\\', '/');
  return name;
}

// -----------------------------------------------------------------------------------------------------------------------------
inline void insertAsset(AssetManager::Assets assets, assets::AssetType type, const std::string& name)
{
  auto pos = assets.lower_bound(type);
  if(pos != assets.end() && !(assets.key_comp()(type, pos->first))) {
    pos->second.insert(name);
  }
  else {
    assets.insert(pos, AssetManager::Assets::value_type(type, { name }));                                
  }
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
AssetManager::AssetManager(const std::string root) :
  root_(root),
  serializer_(nullptr)
{
  addProperty("Assets", &assets_);
  scan();
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetManager::get(const std::string& name)
{
  auto path = root_ / fs::path(name);
  if (fs::is_regular(path)) {
    try {
      fs::ifstream file(path, std::ios::binary | std::ios::in);
      return serializer_->deserialize(file);
    }
    catch (std::exception& exp) {
      LOG(error) << "Cannot load asset \"" << name << "\" (" << exp.what() << ")";
    }
    catch (...) {
      LOG(error) << "Cannot load asset \"" << name << "\"";
    }
  }
  return nullptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::put(assets::Asset* asset)
{
  assets_[asset->type()].insert(asset->name());
  try {
    auto path = root_ / asset->name();
    fs::ofstream file(path, std::ios::binary | std::ios::out);
    serializer_->serialize(asset, file);
  }
  catch (std::exception& exp) {
    LOG(error) << "Cannot save asset \"" << asset->name() << "\" (" << exp.what() << ")";
  }
  catch (...) {
    LOG(error) << "Cannot save asset \"" << asset->name() << "\"";
  }
  properties_[PropertyIndex::assets]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::remove(assets::Asset* asset)
{
  assets_[asset->type()].erase(asset->name());
  try {
    auto path = root_ / asset->name();
    fs::remove(asset->name());
  }
  catch (std::exception& exp) {
    LOG(error) << "Cannot remove asset \"" << asset->name() << "\" (" << exp.what() << ")";
  }
  catch (...) {
    LOG(error) << "Cannot remove asset \"" << asset->name() << "\"";
  }
  properties_[PropertyIndex::assets]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::scan()
{
  assets_.clear();
  auto end = fs::recursive_directory_iterator();
  for (auto it = fs::recursive_directory_iterator(root_); it != end; ++it) {
    const auto& path = it->path();
    if (!fs::is_regular(path)) continue;
    try {
      fs::ifstream file(it->path(), std::ios::binary | std::ios::in);
      auto type = serializer_->check(file);
      auto name = getAssetName(root_, path);     
      insertAsset(assets_, type, name);
    }
    catch (std::exception& exp) {
      LOG(error) << "Cannot read asset \"" << path << "\" (" << exp.what() << ")";
    }
    catch (...) {
      LOG(error) << "Cannot read asset \"" << path << "\"";
    }
  }
  properties_[PropertyIndex::assets]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::pack(const std::string path)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::unpack(const std::string path)
{
}

} // !namespace editor
} // !namespace yaga

#include "precompiled.h"
#include "asset_manager.h"
#include "assets/binary_serializer.h"
#include "asset_type_camera.h"
#include "asset_type_image.h"
#include "asset_type_material.h"
#include "asset_type_mesh.h"
#include "asset_type_model.h"
#include "asset_type_scene.h"
#include "asset_type_shader.h"
#include "asset_type_text.h"
#include "asset_type_texture.h"
#include "asset_type_transform.h"

namespace fs = boost::filesystem;

namespace yaga {
namespace editor {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
std::string getAssetName(const fs::path& root, fs::path path)
{
  auto name = path.lexically_relative(root).string();
  std::replace(name.begin(), name.end(), '\\', '/');
  if (name.empty()) THROW("Empty asset name");
  return name;
}

// -----------------------------------------------------------------------------------------------------------------------------
fs::path normalizeAssetName(const fs::path& root, AssetType* type, std::string& name)
{
  name += "." + type->extension();
  auto path = fs::absolute(fs::path(name), root);
  name = getAssetName(root, path);
  return path;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
AssetManager::AssetManager() :
  serializer_(std::make_unique<assets::BinarySerializer>())
{
  registerType<AssetTypeCamera>();
  registerType<AssetTypeImage>();
  registerType<AssetTypeMaterial>();
  registerType<AssetTypeMesh>();
  registerType<AssetTypeModel>();
  registerType<AssetTypeScene>();
  registerType<AssetTypeShader>();
  registerType<AssetTypeText>();
  registerType<AssetTypeTexture>();
  registerType<AssetTypeTransform>();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::load(const std::string& root)
{
  root_ = fs::path(root) / "assets";
  fs::create_directories(root_);
  scan();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::save()
{
  std::this_thread::sleep_for(std::chrono::seconds(10));
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::registerType(AssetTypePtr&& type)
{
  const auto id = type->id();
  auto it = assets_.find(id);
  if (it != assets_.end()) THROW("Asset type %1% already registered", id);
  std::string ext_str = type->extension();
  if (!ext_str.empty() && ext_str[0] != '.') ext_str = "." + ext_str;
  auto ext = extensions_.find(ext_str);
  if (ext != extensions_.end()) 
    THROW("Asset types %1% and %2% share the same file extension %3%", id, ext->second->id(), ext->second->extension());
  extensions_[ext_str] = type.get();
  AssetTypeStruct typeStruct { std::move(type) };
  assets_[id] = std::move(typeStruct);
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetMetadata* AssetManager::createAsset(AssetTypeId typeId, const std::string& name, const std::string& importFilename)
{
  THROW("KEKEKEKEKEKE");

  auto typeIt = assets_.find(typeId);
  if (typeIt == assets_.end()) THROW("Could not create asset: unknown asset type");
  auto& assets = typeIt->second.assets;
  auto type = typeIt->second.type.get();

  std::string assetName = name;
  auto path = normalizeAssetName(root_, type, assetName);
  if (assets.find(assetName) != assets.end()) THROW("Could not create asset: name already in use");
  auto asset = typeIt->second.type->createAsset(assetName, importFilename);
  if (asset == nullptr) THROW("Could not create asset");

  fs::create_directories(path.parent_path());
  fs::ofstream file(path, std::ios::out | std::ios::binary);
  serializer_->serialize(asset.get(), file);
  file.close();

  auto meta = std::make_unique<AssetMetadata>(this, type, assetName, path);
  auto metaPtr = meta.get();
  typeIt->second.assets[name] = std::move(meta);
  return metaPtr;
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::removeAsset(AssetMetadata* asset)
{
  if (asset->used()) THROW("Could not delete asset: asset in use");
  auto typeIt = assets_.find(asset->type()->id());
  if (typeIt == assets_.end()) THROW("Could not delete asset: not from this asset manager");
  auto& assets = typeIt->second.assets;
  auto it = assets.find(asset->name());
  if (it == assets.end() || it->second.get() != asset) THROW("Could not delete asset: not from this asset manager");
  assets.erase(it);
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetManager::get(AssetTypeId type, const std::string& name)
{
  auto asset = tryGet(type, name);
  if (!asset) THROW("Could not find asset \"%1%\"", name);
  return asset;
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetManager::tryGet(AssetTypeId type, const std::string& name)
{
  auto it = assets_.find(type);
  if (it == assets_.end()) return nullptr;
  auto& assets = it->second.assets;
  auto ait = assets.find(name);
  if (ait == assets.end()) return nullptr;
  return ait->second->load();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::scan()
{
  auto end = fs::recursive_directory_iterator();
  for (auto it = fs::recursive_directory_iterator(root_); it != end; ++it) {
    const auto& path = it->path();
    try {
      auto ext = extensions_.find(path.extension());
      if (ext == extensions_.end()) THROW("Unknown asset extension %1%", path.extension());
      auto typeId = ext->second->id();
      auto name = getAssetName(root_, path);
      auto& type = assets_[typeId];
      if (type.assets.find(name) != type.assets.end()) continue;
      type.assets[name] = std::make_unique<AssetMetadata>(this, type.type.get(), name, path);
    }
    catch (std::exception& exp) {
      LOG(error) << "Could not load asset \"" << path << "\": " << exp.what();
    }
    catch (...) {
      LOG(error) << "Could not load asset \"" << path << "\"";
    }
  }
}

/*

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::pack(const std::string path)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetManager::unpack(const std::string path)
{
}

*/

} // !namespace editor
} // !namespace yaga

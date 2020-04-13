#include "precompiled.h"
#include "asset/serializer.h"
#include "asset/application.h"
#include "asset/image.h"
#include "asset/material.h"
#include "asset/mesh.h"
#include "asset/model.h"
#include "asset/scene.h"
#include "asset/shader.h"
#include "asset/texture.h"
#include "utility/array.h"

namespace yaga
{
namespace asset
{

namespace fs = boost::filesystem;
namespace endian = boost::endian;

namespace
{

// -------------------------------------------------------------------------------------------------------------------------
std::string typeFromPath(const fs::path& path)
{
  auto type = path.extension().string();
  if (type.size() > 0) {
    type = type.substr(1);
    boost::algorithm::to_lower(type);
  }
  return type;
}

} // !namespace

std::map<uint32_t, SerializationInfo> Serializer::serializers_;
std::map<std::string, SerializationInfo> Serializer::serializersFriendly_;

// -------------------------------------------------------------------------------------------------------------------------
void Serializer::registerAsset(const SerializationInfo& info)
{
  auto it = serializers_.find(info.id);
  if (it != serializers_.end()) {
    LOG(warning) << "\"" << info.name << "\" overrides serialization info of \"" << it->second.name << "\", id " << info.id;
  } else {
    for (const auto& id : info.friendlyId) {
      auto itf = serializersFriendly_.find(id);
      if (itf != serializersFriendly_.end()) {
        LOG(warning) << "\"" << info.name << "\" overrides serialization info of \"" << itf->second.name << "\", id " << info.id;
      }
      serializersFriendly_[id] = info;
    }
  }
  serializers_[info.id]  = info;
}

// -------------------------------------------------------------------------------------------------------------------------
void Serializer::registerStandardAssets()
{
  registerAsset<Application>();
  registerAsset<Image>();
  registerAsset<Scene>();
  registerAsset<Shader>();
  registerAsset<Material>();
  registerAsset<Mesh>();
  registerAsset<Model>();
  registerAsset<Texture>();
}

// -------------------------------------------------------------------------------------------------------------------------
void Serializer::deserializeFriendly(const std::string& path, Database* db)
{
  auto parent = fs::path(path);
  fs::recursive_directory_iterator root(path);
  for (auto&& entry : root) {
    if (!fs::is_regular_file(entry)) continue;
    auto type = typeFromPath(entry.path());
    auto name = fs::relative(entry, parent).stem().string();
    auto asset = deserializeFriendly(type, name, entry.path().string());
    if (!asset) {
      LOG(warning) << "Could not load asset of type " << type;
    }
    else {
      db->put(std::move(asset), false);
    }
  }
  db->resolveRefs();
}

// -------------------------------------------------------------------------------------------------------------------------
AssetPtr Serializer::deserializeFriendly(const std::string& type, const std::string& name, const std::string& file)
{
  try {
    auto it = serializersFriendly_.find(type);
    if (it != serializersFriendly_.end()) {
      std::ifstream stream(file, std::ios::ate | std::ios::binary);
      size_t size = static_cast<size_t>(stream.tellg());
      stream.seekg(0);
      return it->second.deserializeFriendly(type, name, stream, size);
    }
  }
  catch (...) {
    LOG(warning) << "Could not load asset " << file;
  }
  return nullptr;
}

// -------------------------------------------------------------------------------------------------------------------------
void Serializer::deserializeBin(const std::string&, Database*)
{
  THROW_NOT_IMPLEMENTED;
}

} // !namespace asset
} // !namespace yaga


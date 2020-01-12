#include "precompiled.h"
#include "serializer.h"
#include "application.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
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

std::map<Serializer::SInfoId, Serializer::SInfo> Serializer::serializers_;
std::map<Serializer::SInfoIdFriendly, Serializer::SInfo> Serializer::serializersFriendly_;

// -------------------------------------------------------------------------------------------------------------------------
void Serializer::registerAsset(const AssetId& id, const SerializeProc& serializer, const DeserializeProc& deserializer)
{
  SInfo info = { id, serializer, deserializer };
  serializers_[id.binary]  = info;
  serializersFriendly_[id.friendly] = info;
}

// -------------------------------------------------------------------------------------------------------------------------
void Serializer::registerStandardAssets()
{
  registerAsset<asset::Application>();
  registerAsset<asset::Shader>();
  registerAsset<asset::Material>();
  registerAsset<asset::Mesh>();
  registerAsset<asset::Texture>();
}

// -------------------------------------------------------------------------------------------------------------------------
void Serializer::deserialize(const std::string& dir, Database* db)
{
  auto parent = fs::path(dir);
  fs::recursive_directory_iterator root(dir);
  for (auto&& entry : root) {
    if (!fs::is_regular_file(entry)) continue;
    auto type = typeFromPath(entry.path());
    auto name = fs::relative(entry, parent).stem().string();
    auto asset = deserialize(type, name, entry.path().string());
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
void Serializer::deserialize(const std::string& name, const std::string& path, Database* db)
{
  auto type = typeFromPath(path);
  auto asset = deserialize(type, name, path);
  if (!asset) {
    LOG(warning) << "Could not load asset of type " << type;
  }
  else {
    db->put(std::move(asset));
  }
}

// -------------------------------------------------------------------------------------------------------------------------
AssetPtr Serializer::deserialize(const std::string& type, const std::string& name, const std::string& file)
{
  try {
    auto it = serializersFriendly_.find(type);
    if (it != serializersFriendly_.end()) {
      std::ifstream stream(file, std::ios::ate | std::ios::binary);
      size_t size = static_cast<size_t>(stream.tellg());
      stream.seekg(0);
      return it->second.deserializer(name, stream, size, false);
    }
  }
  catch (...) {
    LOG(warning) << "Could not load asset " << file;
  }
  return nullptr;
}

// -------------------------------------------------------------------------------------------------------------------------
size_t Serializer::serialize(std::ostream& stream, const std::string& str)
{
  size_t size = str.size();
  endian::native_to_little_inplace(size);
  stream.write((char*)&size, sizeof(size_t));
  stream.write(str.data(), str.size());
  return str.size() + sizeof(size_t);
}

// -------------------------------------------------------------------------------------------------------------------------
std::string Serializer::deserializeString(std::istream& stream)
{
  const constexpr size_t BSIZE = 64;
  size_t size;
  stream.read((char*)&size, sizeof(size_t));
  endian::little_to_native_inplace(size);
  if (size <= BSIZE) {
    char buffer[BSIZE];
    stream.read(buffer, size);
    return std::string(buffer, size);
  }
  ByteArray buffer(size);
  stream.read(buffer.data(), size);
  return std::string(buffer.data(), size);
}

} // !namespace asset
} // !namespace yaga


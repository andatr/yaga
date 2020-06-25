#include "precompiled.h"
#include "assets/friendly_serializer.h"

namespace yaga
{
namespace assets
{

namespace fs = boost::filesystem;

namespace
{

// -------------------------------------------------------------------------------------------------------------------------
std::string typeFromPath(const fs::path& path)
{
  auto type = path.extension().string();
  if (!type.empty()) {
    type = type.substr(1);
    boost::algorithm::to_lower(type);
  }
  return type;
}

// -------------------------------------------------------------------------------------------------------------------------
std::string nameFromPath(const fs::path& path)
{
  std::stringstream name;
  const auto end = --path.end();
  for (auto it = path.begin(); it != end; ++it) {
    name << it->string() << "/";
  }
  name << path.stem().string();
  return name.str();
}

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
FriendlySerializer::FriendlySerializer(const std::string& root)
{
  fs::path fsroot(root);
  fs::recursive_directory_iterator it(fsroot);
  fs::recursive_directory_iterator end;
  while (it != end) {
    if (fs::is_regular(it->path())) {
      auto rel = fs::relative(it->path(), fsroot);
      auto name = nameFromPath(rel);
      auto type = typeFromPath(rel);
      assets_[name] = AssetInfo(type, it->path().string());
    }
    ++it;
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void FriendlySerializer::registerAsset(const SerializationInfo& info)
{
  for (const auto& id : info.friendlyId) {
    auto it = serializers_.find(id);
    if (it != serializers_.end()) {
      THROW("Serializer for type %1% already exists", id);
    }
    serializers_[id] = info;
  }
}

// -------------------------------------------------------------------------------------------------------------------------
AssetPtr FriendlySerializer::deserialize(const std::string& name, RefResolver& resolver)
{
  auto it = assets_.find(name);
  if (it == assets_.end()) {
    THROW("Could not find asset %1%", name);
  }
  const auto& info = it->second;
  auto serializer = serializers_.find(info.type);
  if (serializer == serializers_.end()) {
    THROW("Unknown asset type %1%", info.type);
  }
  return serializer->second.deserializeFriendly(name, info.path, resolver);
}

} // !namespace assets
} // !namespace yaga


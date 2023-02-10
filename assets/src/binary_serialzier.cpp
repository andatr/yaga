#include "precompiled.h"
#include "assets/binary_serializer.h"
#include "binary_serializer_registry.h"

namespace yaga {
namespace assets {
namespace {

#pragma pack(push, 1)

struct Header
{
  char magic[4];
  uint32_t type;
};

#pragma pack(pop)

constexpr const char MAGIC[5] = "YAGA";

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
AssetPtr BinarySerializer::deserialize(std::istream& stream)
{
  Header header{};
  stream.read((char*)&header, sizeof(Header)); // assume proper endianness
  auto sinfo = BinarySerializerRegistry::instance().get(header.type);
  if (!stream.good() || strncmp(header.magic, MAGIC, sizeof(header.magic)) != 0 || !sinfo) {
    THROW("Could not deserialize asset");
  }
  return sinfo->deserialize(stream);  
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetType BinarySerializer::check(std::istream& stream)
{
  Header header{};
  stream.read((char*)&header, sizeof(Header));
  auto sinfo = BinarySerializerRegistry::instance().get(header.type);
  if (!stream.good() || strncmp(header.magic, MAGIC, sizeof(header.magic)) != 0 || !sinfo) {
    THROW("Could not deserialize asset");
  }
  stream.seekg(-sizeof(Header), std::ios::cur);
  return static_cast<AssetType>(header.type);
}

// -----------------------------------------------------------------------------------------------------------------------------
void BinarySerializer::serialize(Asset* asset, std::ostream& stream)
{
  Header header{};
  strncpy(header.magic, MAGIC, sizeof(header.magic));
  header.type = asset->type();
  auto sinfo = BinarySerializerRegistry::instance().get(header.type);
  if (!sinfo || !stream.good()) {
    THROW("Could not serialize asset");
  }
  stream.write((char*)&header, sizeof(Header));
  sinfo->serialize(asset, stream);
}

} // !namespace assets
} // !namespace yaga

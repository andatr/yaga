#include "precompiled.h"
#include "binary_serializer_registry.h"

namespace yaga {
namespace assets {

// -----------------------------------------------------------------------------------------------------------------------------
BinarySerializerRegistry& BinarySerializerRegistry::instance()
{
  static BinarySerializerRegistry inst;
  return inst;
}

// -----------------------------------------------------------------------------------------------------------------------------
BinarySerializerRegistry::SerializeInfo* BinarySerializerRegistry::get(AssetType type)
{
  auto it = storage_.find(type);
  if (it == storage_.end()) {
    return nullptr;
  }
  return &it->second;
}

// -----------------------------------------------------------------------------------------------------------------------------
void BinarySerializerRegistry::add(AssetType type, DeserializeFunc deserialize, SerializeFunc serialize)
{
  storage_[type] = { deserialize, serialize };
}

} // !namespace assets
} // !namespace yaga

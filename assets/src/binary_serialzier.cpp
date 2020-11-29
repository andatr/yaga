#include "precompiled.h"
#include "assets/binary_serializer.h"

namespace yaga {
namespace assets {

namespace endian = boost::endian;

// -----------------------------------------------------------------------------------------------------------------------------
BinarySerializer::BinarySerializer(const std::string&)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void BinarySerializer::registerAsset(const SerializationInfo& info)
{
  auto it = serializers_.find(info.id);
  if (it != serializers_.end()) {
    THROW("Serializer for type %1% already exists", info.id);
  }
  serializers_[info.id] = info;
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetPtr BinarySerializer::deserialize(const std::string&, RefResolver&)
{
  THROW_NOT_IMPLEMENTED;
}

} // !namespace assets
} // !namespace yaga

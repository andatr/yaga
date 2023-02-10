#ifndef YAGA_ASSETS_BINARY_SERIALIZER_REGISTRY
#define YAGA_ASSETS_BINARY_SERIALIZER_REGISTRY

#include "utility/compiler.h"

#include <functional>
#include <map>
#include <istream>
#include <memory>
#include <ostream>
#include <string>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "assets/asset.h"
#include "assets/serializer.h"

#define BINARY_SERIALIZER_REG(E)                           \
  template<>                                               \
  struct BinarySerializerRegistryRegistration<E>           \
  {                                                        \
    static const BinarySerializerRegistryEntry<E>& entry;  \
  };                                                       \
  const BinarySerializerRegistryEntry<E>&                  \
    BinarySerializerRegistryRegistration<E>::entry =       \
      BinarySerializerRegistryEntry<E>::instance();        \

namespace yaga {
namespace assets {

typedef std::function<AssetPtr(std::istream&)> DeserializeFunc;
typedef std::function<void(Asset*, std::ostream&)> SerializeFunc;

// -----------------------------------------------------------------------------------------------------------------------------
class BinarySerializerRegistry : private boost::noncopyable
{
public:
  struct SerializeInfo
  {
    DeserializeFunc deserialize;
    SerializeFunc   serialize;
  };

public:
  static BinarySerializerRegistry& instance();
  SerializeInfo* get(AssetType type);
  void add(AssetType type, DeserializeFunc deserialize, SerializeFunc serialize);

private:
  BinarySerializerRegistry() {}

private:
  std::map<AssetType, SerializeInfo> storage_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Asset>
class BinarySerializerRegistryEntry : private boost::noncopyable
{
public:
  static BinarySerializerRegistryEntry<Asset>& instance();

private:
  BinarySerializerRegistryEntry();
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Asset>
struct BinarySerializerRegistryRegistration
{
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Asset>
BinarySerializerRegistryEntry<Asset>& BinarySerializerRegistryEntry<Asset>::instance()
{
  static BinarySerializerRegistryEntry<Asset> inst;
  return inst;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Asset>
BinarySerializerRegistryEntry<Asset>::BinarySerializerRegistryEntry()
{
  BinarySerializerRegistry::instance().add(Asset::typeId, Asset::deserializeBinary, Asset::serializeBinary);
}

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_BINARY_SERIALIZER_REGISTRY

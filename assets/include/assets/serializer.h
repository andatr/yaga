#ifndef YAGA_ASSETS_SERIALIZER
#define YAGA_ASSETS_SERIALIZER

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "assets/asset.h"
#include "assets/storage.h"

namespace yaga {
namespace assets {

typedef uint32_t AssetId;

enum class StandardAssetId : AssetId
{
  application = 1,
  shader = 2,
  image = 3,
  mesh = 4,
  texture = 5,
  material = 6,
  model = 7,
  scene = 8,
  camera = 9,
  library = 10
};

class Serializer;
typedef std::unique_ptr<Serializer> SerializerPtr;

class RefResolver
{
public:
  RefResolver(Storage* storage, Serializer* serializer);
  Asset* getAsset(const std::string& name);
  template <typename T>
  T* getAsset(const std::string& name);

private:
  Storage* storage_;
  Serializer* serializer_;
};

typedef std::function<AssetPtr(const std::string& name, std::istream& stream, size_t size, RefResolver& resolver)>
  DeserializeBinaryProc;

typedef std::function<AssetPtr(const std::string& name, const std::string& path, RefResolver& resolver)>
  DeserializeFriendlyProc;

struct SerializationInfo
{
  AssetId id;
  std::vector<std::string> friendlyId;
  DeserializeBinaryProc deserializeBinary;
  DeserializeFriendlyProc deserializeFriendly;
};

class Serializer
{
  friend class RefResolver;

public:
  virtual void registerAsset(const SerializationInfo& info) = 0;
  template <typename T>
  void registerAsset();
  void registerStandardAssets();

public:
  Asset* deserialize(const std::string& name, Storage* storage);
  template <typename T>
  T* deserialize(const std::string& name, Storage* storage);

private:
  virtual AssetPtr deserialize(const std::string& name, RefResolver& resolver) = 0;
};

// -------------------------------------------------------------------------------------------------------------------------
template <typename T>
T* RefResolver::getAsset(const std::string& name)
{
  return static_cast<T*>(getAsset(name));
}

// -------------------------------------------------------------------------------------------------------------------------
template <typename T>
T* Serializer::deserialize(const std::string& name, Storage* storage)
{
  return static_cast<T*>(deserialize(name, storage));
}

// -------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Serializer::registerAsset()
{
  registerAsset(T::serializationInfo);
}

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_SERIALIZER

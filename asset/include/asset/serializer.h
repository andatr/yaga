#ifndef YAGA_ASSET_SERIALIZER
#define YAGA_ASSET_SERIALIZER

#include <map>
#include <string>
#include <functional>

#include "database.h"

namespace yaga
{
namespace asset
{

struct AssetId
{
  std::string friendly;
  uint32_t binary;
};

class Serializer
{
public:
  typedef std::function<size_t(Asset* asset, std::ostream& stream, bool binary)> SerializeProc;
  typedef std::function<AssetPtr(const std::string& name, std::istream& stream, size_t size, bool binary)> DeserializeProc;
public:
  static void registerAsset(const AssetId& id, const SerializeProc& serializer, const DeserializeProc& deserializer);
  template<typename T>
  static void registerAsset();
  static void registerStandardAssets();
  static size_t serialize(std::ostream& stream, const std::string& str);
  static std::string deserializeString(std::istream& stream);
  static void deserialize(const std::string& name, const std::string& path, Database* db);
  static void deserialize(const std::string& dir, Database* db);
private:
  static AssetPtr deserialize(const std::string& type, const std::string& name, const std::string& file);
private:
  class SInfo
  {
  public:
    AssetId id;
    SerializeProc serializer;
    DeserializeProc deserializer;
  };
  typedef decltype(AssetId::binary) SInfoId;
  typedef decltype(AssetId::friendly) SInfoIdFriendly;
private:
  static std::map<SInfoId, SInfo> serializers_;
  static std::map<SInfoIdFriendly, SInfo> serializersFriendly_;
};

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
void Serializer::registerAsset()
{
  Serializer::registerAsset(T::assetId, T::serialize, T::deserialize);
}

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_SERIALIZER

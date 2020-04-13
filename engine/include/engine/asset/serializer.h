#ifndef YAGA_ENGINE_ASSET_SERIALIZER
#define YAGA_ENGINE_ASSET_SERIALIZER

#include <map>
#include <string>
#include <functional>

#include "engine/asset/database.h"

namespace yaga
{
namespace asset
{

// 1 Application
// 2 Shader
// 3 Image
// 4 Mesh
// 5 Texture
// 6 Material
// 7 Model
// 8 Scene
// 9 Camera

struct SerializationInfo
{
  typedef std::function<AssetPtr(const std::string& name, std::istream& stream, size_t size)> DeserializeProc;
  typedef std::function<AssetPtr(const std::string& type, const std::string& name, std::istream& stream, size_t size)> DeserializeFriendlyProc;
  
  uint32_t id;
  std::vector<std::string> friendlyId;
  std::string name;  
  DeserializeProc deserialize;
  DeserializeFriendlyProc deserializeFriendly;
};

class Serializer
{
public:
  static void registerAsset(const SerializationInfo& info);
  template<typename T>
  static void registerAsset();
  static void registerStandardAssets();
  static void deserializeFriendly(const std::string& path, Database* db);
  static void deserializeBin(const std::string& path, Database* db);
private:
  static AssetPtr deserializeFriendly(const std::string& type, const std::string& name, const std::string& file);
private:
  static std::map<uint32_t, SerializationInfo> serializers_;
  static std::map<std::string, SerializationInfo> serializersFriendly_;
};

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
void Serializer::registerAsset()
{
  Serializer::registerAsset(T::serializationInfo);
}

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_SERIALIZER

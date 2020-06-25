#ifndef YAGA_ASSETS_BINARY_SERIALIZER
#define YAGA_ASSETS_BINARY_SERIALIZER

#include <map>

#include "assets/serializer.h"

namespace yaga
{
namespace assets
{

class BinarySerializer : public Serializer
{
public:
  explicit BinarySerializer(const std::string& file);
  void registerAsset(const SerializationInfo& info) override;
private:
  AssetPtr deserialize(const std::string& name, RefResolver& resolver) override;
private:
  std::map<AssetId, SerializationInfo> serializers_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_BINARY_SERIALIZER

#ifndef YAGA_ASSETS_FRIENDLY_SERIALIZER
#define YAGA_ASSETS_FRIENDLY_SERIALIZER

#include <map>
#include <string>
#include <boost/filesystem.hpp>

#include "assets/serializer.h"

namespace yaga {
namespace assets {

class FriendlySerializer : public Serializer
{
public:
  explicit FriendlySerializer(const std::string& root);
  void registerAsset(const SerializationInfo& info) override;

private:
  AssetPtr deserialize(const std::string& name, RefResolver& resolver) override;

private:
  struct AssetInfo
  {
    std::string type;
    std::string path;
    AssetInfo() {}
    AssetInfo(const std::string& t, const std::string& p) : type(t), path(p) {}
  };

private:
  std::map<std::string, SerializationInfo> serializers_;
  std::map<std::string, AssetInfo> assets_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_FRIENDLY_SERIALIZER

#ifndef YAGA_ASSETS_BINARY_SERIALIZER
#define YAGA_ASSETS_BINARY_SERIALIZER

#include "assets/serializer.h"

namespace yaga {
namespace assets {

class BinarySerializer : public Serializer
{
public:
  AssetPtr deserialize(std::istream& stream) override;
  void serialize(Asset* asset, std::ostream& stream) override;
  AssetType check(std::istream& stream) override;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_BINARY_SERIALIZER

#ifndef YAGA_ASSETS_SERIALIZER
#define YAGA_ASSETS_SERIALIZER

#include <istream>
#include <ostream>
#include <memory>

#include "assets/asset.h"
#include "assets/storage.h"

namespace yaga {
namespace assets {

class Serializer
{
public:
  virtual ~Serializer() {}

public:
  virtual AssetPtr deserialize(std::istream& stream) = 0;
  template <typename T>
  std::shared_ptr<T> deserialize(std::istream& stream);
  virtual void serialize(Asset* asset, std::ostream& stream) = 0;
  virtual AssetType check(std::istream& stream) = 0;
};

typedef std::unique_ptr<Serializer> SerializerPtr;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::shared_ptr<T> Serializer::deserialize(std::istream& stream)
{
  return std::static_pointer_cast<T>(deserialize(stream));
}

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_SERIALIZER

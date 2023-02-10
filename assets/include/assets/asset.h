#ifndef YAGA_ASSETS_ASSET
#define YAGA_ASSETS_ASSET

#include "utility/compiler.h"

#include <memory>
#include <string>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "utility/prop_info.h"

namespace yaga {
namespace assets {

typedef uint32_t AssetType;

enum class StandardAssetType : AssetType
{
  transform = 1,
  camera    = 2,
  shader    = 3,
  image     = 4,
  texture   = 5,
  material  = 6,
  mesh      = 7,
  model     = 8,
  scene     = 9,
  text      = 10
};

class Storage;

class Asset
  : private boost::noncopyable
  , public PropInfo
{
friend class Serializer;

public:
  explicit Asset(const std::string& name);
  virtual ~Asset() {}
  const std::string& name() const { return name_; }
  virtual AssetType type() const = 0;

protected:
  const std::string name_;
};

typedef std::shared_ptr<Asset> AssetPtr;

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_ASSET

#ifndef YAGA_ASSETS_ASSET
#define YAGA_ASSETS_ASSET

#include <memory>
#include <string>
#include <boost/noncopyable.hpp>

namespace yaga {
namespace assets {

class Asset : private boost::noncopyable
{
  friend class Serializer;

public:
  explicit Asset(const std::string& name);
  virtual ~Asset() {}
  const std::string& name() const { return name_; }

protected:
  const std::string name_;
};

typedef std::unique_ptr<Asset> AssetPtr;

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_ASSET

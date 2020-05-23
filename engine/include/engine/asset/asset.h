#ifndef YAGA_ENGINE_ASSET_ASSET
#define YAGA_ENGINE_ASSET_ASSET

#include <memory>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

namespace yaga 
{
namespace asset
{

class Asset : private boost::noncopyable
{
friend class Database;
friend class Serializer;
public:
  explicit Asset(const std::string& name);
  virtual ~Asset() {}
  const std::string& name() const { return name_; }
protected:
  virtual void resolveRefs(Database* db);
protected:
  const std::string name_;
};

typedef std::unique_ptr<Asset> AssetPtr;

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_ASSET


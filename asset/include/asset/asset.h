#ifndef YAGA_ASSET_ASSET
#define YAGA_ASSET_ASSET

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
public:
  explicit Asset(const std::string& name);
  virtual ~Asset() {}
  const std::string& name() const { return name_; }
protected:
  friend class Database;
  virtual void resolveRefs(Database* db);
protected:
  const std::string name_;
};

typedef std::unique_ptr<Asset> AssetPtr;

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_ASSET


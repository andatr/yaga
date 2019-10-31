#ifndef YAGA_ASSET_ASSET
#define YAGA_ASSET_ASSET

#include <memory>
#include <string>
#include <vector>

namespace yaga 
{
namespace asset
{

class Asset
{
public:
  explicit Asset(const std::string& name);
  virtual ~Asset() {}
  const std::string& Name() const { return name_; }
protected:
  friend class Database;
  virtual void ResolveRefs(Database* db);
protected:
  const std::string name_;
};

typedef std::unique_ptr<Asset> AssetPtr;

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_ASSET

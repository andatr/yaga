#ifndef YAGA_ASSET_DATABASE
#define YAGA_ASSET_DATABASE

#include <map>
#include <memory>
#include <string>

#include "asset.h"
#include "utility/exception.h"

namespace yaga 
{
namespace asset
{

class Database
{
public:
  Asset* Get(const std::string& name);
  void Put(AssetPtr asset, bool resolveRefs = true);
  void Delete(AssetPtr asset);
  void Delete(const std::string& name);
  void ResolveRefs();
  template<typename T>
  T* Get(const std::string& name);
private:
  std::map<std::string, AssetPtr> assets_;
};

typedef std::unique_ptr<Database> DatabasePtr;

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
T* Database::Get(const std::string& name)
{
  auto ptr = dynamic_cast<T*>(Get(name));
  if (!ptr) {
    THROW("Wrong asset type \"%1\"", name);
  }
  return ptr;
}

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_DATABASE

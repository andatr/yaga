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
  Asset* get(const std::string& name);
  void put(AssetPtr asset, bool resolveRefs = true);
  void remove(AssetPtr asset);
  void remove(const std::string& name);
  void resolveRefs();
  template<typename T>
  T* get(const std::string& name);
private:
  std::map<std::string, AssetPtr> assets_;
};

typedef std::unique_ptr<Database> DatabasePtr;

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
T* Database::get(const std::string& name)
{
  auto ptr = dynamic_cast<T*>(get(name));
  if (!ptr) {
    THROW("Wrong asset type \"%1\"", name);
  }
  return ptr;
}

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_DATABASE

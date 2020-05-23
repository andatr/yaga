#ifndef YAGA_ENGINE_DATABASE
#define YAGA_ENGINE_DATABASE

#include <map>
#include <memory>
#include <string>

#include "engine/asset/asset.h"
#include "utility/exception.h"

namespace yaga 
{
namespace asset
{

class Database
{
public:
  Asset* get(const std::string& name) const;
  void put(AssetPtr asset, bool resolveRefs = true);
  void remove(Asset* asset);
  void remove(const std::string& name);
  void resolveRefs();
  template<typename T>
  T* get(const std::string& name) const;
private:
  std::map<std::string, AssetPtr> assets_;
};

typedef std::unique_ptr<Database> DatabasePtr;

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
T* Database::get(const std::string& name) const
{
  auto ptr = dynamic_cast<T*>(get(name));
  if (!ptr) {
    THROW("Wrong asset type \"%1%\"", name);
  }
  return ptr;
}

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_DATABASE

#ifndef YAGA_ASSETS_STORAGE
#define YAGA_ASSETS_STORAGE

#include <map>
#include <memory>
#include <string>

#include "assets/asset.h"
#include "utility/exception.h"

namespace yaga {
namespace assets {

class Storage
{
public:
  Asset* get(const std::string& name) const;
  Asset* tryGet(const std::string& name) const;
  void put(AssetPtr asset);
  void remove(Asset* asset);
  void remove(const std::string& name);
  template <typename T>
  T* get(const std::string& name) const;
  template <typename T>
  T* tryGet(const std::string& name) const;

private:
  std::map<std::string, AssetPtr> assets_;
};

typedef std::unique_ptr<Storage> StoragePtr;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T* Storage::get(const std::string& name) const
{
  auto ptr = dynamic_cast<T*>(get(name));
  if (!ptr) {
    THROW("Wrong asset type \"%1%\"", name);
  }
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T* Storage::tryGet(const std::string& name) const
{
  return dynamic_cast<T*>(tryGet(name));
}

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_STORAGE

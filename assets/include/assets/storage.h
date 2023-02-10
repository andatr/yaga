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
  virtual ~Storage() {};
  virtual AssetPtr get(AssetType type, const std::string& name) = 0;
  virtual AssetPtr tryGet(AssetType type, const std::string& name) = 0;
  template <typename T>
  std::shared_ptr<T> get(const std::string& name);
  template <typename T>
  std::shared_ptr<T> tryGet(const std::string& name);
};

typedef std::unique_ptr<Storage> StoragePtr;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::shared_ptr<T> Storage::get(const std::string& name)
{
  return std::static_pointer_cast<T>(get(T::typeId, name));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::shared_ptr<T> Storage::tryGet(const std::string& name)
{
  return std::static_pointer_cast<T>(tryGet(T::typeId, name));
}

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_STORAGE

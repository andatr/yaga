#ifndef YAGA_EDITOR_SRC_ASSET_REGISTRY
#define YAGA_EDITOR_SRC_ASSET_REGISTRY

#include "utility/compiler.h"

#include <memory>
#include <string>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "assets/asset.h"

#define ASSET_REGISTRY(N, A)                    \
  template<>                                    \
  struct AssetRegistryRegistration<A>           \
  {                                             \
    static const AssetRegistryEntry<A>& entry;  \
  };                                            \
  const AssetRegistryEntry<A>&                  \
    AssetRegistryRegistration<A>::entry =       \
      AssetRegistryEntry<A>::instance(N);       \

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
class AssetRegistry : private boost::noncopyable
{
public:
  struct AssetInfo
  {
    const char* name;
  };

public:
  static AssetRegistry& instance();
  const std::map<assets::AssetType, AssetInfo>& assets() { return storage_; }
  void add(const char* name, assets::AssetType type);

private:
  AssetRegistry() {}

private:
  std::map<assets::AssetType, AssetInfo> storage_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Asset>
class AssetRegistryEntry : private boost::noncopyable
{
public:
  static AssetRegistryEntry<Asset>& instance(const char* name);

private:
  AssetRegistryEntry(const char* name = nullptr);
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Asset>
struct AssetRegistryRegistration
{
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Asset>
AssetRegistryEntry<Asset>& AssetRegistryEntry<Asset>::instance(const char* name)
{
  static AssetRegistryEntry<Asset> inst(name);
  return inst;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Asset>
AssetRegistryEntry<Asset>::AssetRegistryEntry(const char* name)
{
  AssetRegistry::instance().add(name, Asset::typeId);
}

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_REGISTRY

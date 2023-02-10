#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_MANAGER
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_MANAGER

#include "utility/compiler.h"

#include <memory>
#include <string>

DISABLE_WARNINGS
#include <boost/filesystem.hpp>
ENABLE_WARNINGS

#include "asset_metadata.h"
#include "asset_type.h"
#include "assets/serializer.h"
#include "assets/storage.h"

namespace yaga {
namespace editor {

class AssetManager : public assets::Storage
{
public:
  struct AssetTypeStruct
  {
    AssetTypePtr type;
    std::map<std::string, AssetMetadataPtr> assets;
  };
  typedef std::map<AssetTypeId, AssetTypeStruct> Assets;
  struct PropertyIndex
  {
    static const int assetTypes = 0;
    static const int assets     = 1;
  };

public:
  explicit AssetManager();
  virtual ~AssetManager() {}
  void registerType(AssetTypePtr&& type);
  template<typename T>
  void registerType();
  void load(const std::string& root);
  void save();
  AssetMetadata* createAsset(AssetTypeId typeId, const std::string& name, const std::string& importFilename);
  void removeAsset(AssetMetadata* asset);
  const Assets& assets() const { return assets_; }
  void scan();
  assets::Serializer* serializer() const { return serializer_.get(); }
  assets::AssetPtr get(AssetTypeId type, const std::string& name) override;
  assets::AssetPtr tryGet(AssetTypeId type, const std::string& name) override;
  
  /*
 
  void remove(assets::Asset* asset);
  void pack(const std::string path);
  void unpack(const std::string path);*/

private:
  boost::filesystem::path root_;
  assets::SerializerPtr serializer_;
  Assets assets_;
  std::map<boost::filesystem::path, AssetType*> extensions_;
};

typedef std::unique_ptr<AssetManager> AssetManagerPtr;

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
void AssetManager::registerType()
{
  registerType(std::make_unique<T>());
}

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_MANAGER

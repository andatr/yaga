#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_METADATA
#define YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_METADATA

#include "utility/compiler.h"

#include <memory>
#include <string>

DISABLE_WARNINGS
#include <boost/filesystem.hpp>
ENABLE_WARNINGS

#include "assets/asset.h"
#include "assets/serializer.h"

namespace yaga {
namespace editor {

class AssetManager;
class AssetType;

class AssetMetadata
{
public:
  explicit AssetMetadata(AssetManager* assetManager, AssetType* type, const std::string& name, const boost::filesystem::path& path);
  virtual ~AssetMetadata() {}
  const std::string& name() const { return name_; }
  const boost::filesystem::path& path() const { return path_; }
  AssetType* type() const { return type_; }
  assets::AssetPtr load();
  void rename(const std::string& name);
  void remove();
  bool used() { return false; }

private:
  void unload();

private:
  AssetManager* assetManager_;
  AssetType* type_;
  std::string name_;
  boost::filesystem::path path_;
  assets::AssetPtr asset_;
};

typedef std::unique_ptr<AssetMetadata> AssetMetadataPtr;

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER_ASSET_METADATA

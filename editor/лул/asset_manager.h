#ifndef YAGA_EDITOR_SRC_ASSET_MANAGER
#define YAGA_EDITOR_SRC_ASSET_MANAGER

#include "utility/compiler.h"

#include <memory>
#include <string>
#include <typeindex>

DISABLE_WARNINGS
#include <boost/filesystem.hpp>
ENABLE_WARNINGS

#include "asset_view_data.h"
#include "assets/asset.h"
#include "assets/serializer.h"


namespace yaga {
namespace editor {

class AssetManager : public AssetViewData
{
public:
  explicit AssetManager(const std::string root);
  virtual ~AssetManager() {}
  assets::AssetPtr get(const std::string& name);
  void put(assets::Asset* asset);
  void remove(assets::Asset* asset);
  void pack(const std::string path);
  void unpack(const std::string path);
  const Assets& assets() const override { return assets_; }

private:
  void scan();

private:
  boost::filesystem::path root_;
  assets::Serializer* serializer_;
  Assets assets_;
};

typedef std::unique_ptr<AssetManager> AssetManagerPtr;

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_MANAGER

#include "precompiled.h"
#include "asset_metadata.h"
#include "asset_manager.h"
#include "asset_type.h"

namespace fs = boost::filesystem;

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
AssetMetadata::AssetMetadata(
  AssetManager* assetManager,
  AssetType* type,
  const std::string& name,
  const boost::filesystem::path& path
) :
  assetManager_(assetManager),
  type_(type),
  name_(name),
  path_(path)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetMetadata::load() 
{
  if (asset_) return asset_;
  fs::ifstream file(path_, fs::ifstream::in | fs::ifstream::binary);
  auto asset = assetManager_->serializer()->deserialize(file);
  return nullptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetMetadata::unload()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetMetadata::rename(const std::string&)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetMetadata::remove()
{
}

} // !namespace editor
} // !namespace yaga

#ifndef YAGA_ASSET_TEXTURE
#define YAGA_ASSET_TEXTURE

#include <memory>
#include <istream>
#include <ostream>

#include "asset.h"
#include "serializer.h"

namespace yaga
{
namespace asset
{

class Texture;
typedef std::unique_ptr<Texture> TexturePtr;

class Texture : public Asset
{
public:
  explicit Texture(const std::string& name);
  virtual ~Texture();
  int width() const { return width_; }
  int height() const { return height_; }
  int channels() const { return channels_; }
  size_t Size() const { return size_; }
  const std::string& filename() const { return filename_; }
  const char* data() const { return bytes_; }
public: // Serialization
  static const AssetId assetId;
  static size_t serialize(Asset* asset, std::ostream& stream, bool binary);
  static TexturePtr deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
private:
  char* bytes_;
  int width_;
  int height_;
  int channels_;
  size_t size_;
  std::string filename_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_TEXTURE

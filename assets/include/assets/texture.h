#ifndef YAGA_ASSETS_TEXTURE
#define YAGA_ASSETS_TEXTURE

#include <istream>
#include <memory>
#include <string>

#include "assets/asset.h"
#include "assets/image.h"
#include "assets/serializer.h"

namespace yaga {
namespace assets {

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;

class Texture : public Asset
{
public:
  explicit Texture(const std::string& name);
  virtual ~Texture();
  ImagePtr image() const { return image_; }
  //void image(Image* image);
  AssetType type() const override { return typeId; }

public:
  static const AssetType typeId = (uint32_t)StandardAssetType::texture;
  static TexturePtr deserializeBinary  (std::istream& stream);
  static TexturePtr deserializeFriendly(std::istream& stream);
  static void serializeBinary  (Asset* asset, std::ostream& stream);
  static void serializeFriendly(Asset* asset, std::ostream& stream);
  static void resolveRefs(Asset* asset, Storage* storage);

private:
  ImagePtr image_;
  std::string imageName_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_TEXTURE

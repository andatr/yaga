#ifndef YAGA_ASSETS_TEXTURE
#define YAGA_ASSETS_TEXTURE

#include <istream>
#include <memory>

#include "assets/asset.h"
#include "assets/image.h"
#include "assets/serializer.h"
#include "utility/update_notifier.h"

namespace yaga {
namespace assets {

class Texture;
typedef std::unique_ptr<Texture> TexturePtr;

enum class TextureProperty
{
  image
};

class Texture
  : public Asset
  , public UpdateNotifier<TextureProperty>
{
public:
  explicit Texture(const std::string& name);
  virtual ~Texture();
  Image* image() const { return image_; }
  void image(Image* image);

public:
  static const SerializationInfo serializationInfo;
  static TexturePtr deserializeBinary(const std::string& name, std::istream& stream, size_t size, RefResolver& resolver);
  static TexturePtr deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver);

private:
  Image* image_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_TEXTURE

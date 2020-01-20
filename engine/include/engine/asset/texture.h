#ifndef YAGA_ENGINE_ASSET_TEXTURE
#define YAGA_ENGINE_ASSET_TEXTURE

#include <memory>
#include <istream>
#include <ostream>

#include "engine/asset/asset.h"
#include "engine/asset/image.h"
#include "engine/asset/serializer.h"

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
  Image* image() const { return image_; }
  Texture& image(Image* image);
protected:
  void resolveRefs(Database*) override;
public:
  static const SerializationInfo serializationInfo;
  static TexturePtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static TexturePtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
private:
  Image* image_;
  std::string imageName_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_TEXTURE

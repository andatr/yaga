#include "precompiled.h"
#include "assets/texture.h"

namespace yaga {
namespace assets {

const SerializationInfo Texture::serializationInfo = {
  (uint32_t)StandardAssetId::texture,
  { "ytex" },
  &Texture::deserializeBinary,
  &Texture::deserializeFriendly
};

// -------------------------------------------------------------------------------------------------------------------------
Texture::Texture(const std::string& name) : Asset(name), image_(nullptr)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Texture::image(Image* image)
{
  image_ = image;
  fireUpdate(TextureProperty::image);
}

// -------------------------------------------------------------------------------------------------------------------------
TexturePtr Texture::deserializeBinary(const std::string&, std::istream&, size_t, RefResolver&)
{
  THROW_NOT_IMPLEMENTED;
}

// -------------------------------------------------------------------------------------------------------------------------
TexturePtr Texture::deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver)
{
  namespace pt = boost::property_tree;
  auto texture = std::make_unique<Texture>(name);
  pt::ptree props;
  pt::read_json(path, props);
  texture->image_ = resolver.getAsset<Image>(props.get<std::string>("image"));
  return texture;
}

} // !namespace assets
} // !namespace yaga

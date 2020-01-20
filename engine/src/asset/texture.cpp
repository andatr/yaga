#include "precompiled.h"
#include "asset/texture.h"

namespace yaga
{
namespace asset
{

const SerializationInfo Texture::serializationInfo = {
  5,
  { "ytex" },
  "Texture",
  &Texture::deserialize,
  &Texture::deserializeFriendly
};

// -------------------------------------------------------------------------------------------------------------------------
Texture::Texture(const std::string& name) :
  Asset(name), image_(nullptr)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Texture::resolveRefs(Database* db)
{
  image_ = db->get<Image>(imageName_);
}

// -------------------------------------------------------------------------------------------------------------------------
Texture& Texture::image(Image* image)
{
  image_ = image;
  imageName_ = image->name();
  return *this;
}

// -------------------------------------------------------------------------------------------------------------------------
TexturePtr Texture::deserialize(const std::string&, std::istream&, size_t)
{
  THROW_NOT_IMPLEMENTED; 
}

// -------------------------------------------------------------------------------------------------------------------------
TexturePtr Texture::deserializeFriendly(const std::string&, const std::string& name, std::istream& stream, size_t)
{
  namespace pt = boost::property_tree;
  auto texture = std::make_unique<Texture>(name);
  pt::ptree props;
  pt::read_ini(stream, props);
  texture->imageName_ = props.get<std::string>("image");
  return texture;
}

} // !namespace asset
} // !namespace yaga

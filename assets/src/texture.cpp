#include "precompiled.h"
#include "assets/texture.h"
#include "assets/storage.h"
#include "binary_serializer_helper.h"
#include "binary_serializer_registry.h"
#include "friendly_serializer_helper.h"

namespace yaga {
namespace assets {
namespace {

constexpr const char* IMAGE_PNAME = "vertexShader";

} // !namespace

BINARY_SERIALIZER_REG(Texture)

// -----------------------------------------------------------------------------------------------------------------------------
Texture::Texture(const std::string& name) :
  Asset(name),
  image_(nullptr)
{
  addProperty("Image", image_);
}

// -----------------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
//void Texture::image(Image* image)
//{
//  image_ = image;
//  imageName_ = image ? image->name() : "";
//  properties_[PropertyInfo::image]->update(this);
//}

// -----------------------------------------------------------------------------------------------------------------------------
TexturePtr Texture::deserializeBinary(std::istream& stream)
{
  std::string name;
  binser::read(stream, name);
  auto texture = std::make_unique<Texture>(name);
  binser::read(stream, texture->imageName_);
  return texture;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Texture::serializeBinary(Asset* asset, std::ostream& stream)
{
  auto texture = assetCast<Texture>(asset);
  binser::write(stream, texture->name_);
  binser::write(stream, texture->imageName_);
}

// -----------------------------------------------------------------------------------------------------------------------------
TexturePtr Texture::deserializeFriendly(std::istream& stream)
{
  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_json(stream, props);
  auto texture = frser::createAsset<Texture>(props);
  frser::read(props, IMAGE_PNAME, texture->imageName_);
  return texture;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Texture::serializeFriendly(Asset* asset, std::ostream& stream)
{
  namespace pt = boost::property_tree;
  auto texture = assetCast<Texture>(asset);
  pt::ptree props;
  frser::write(props, frser::NAME_PNAME, texture->name_);
  frser::write(props, IMAGE_PNAME,       texture->imageName_);
  pt::write_json(stream, props);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Texture::resolveRefs(Asset* asset, Storage* storage)
{
  auto texture = assetCast<Texture>(asset);
  if (!texture->name_.empty()) {
    texture->image_ = storage->get<Image>(texture->imageName_);
  }
}

} // !namespace assets
} // !namespace yaga

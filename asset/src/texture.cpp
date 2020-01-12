#include "precompiled.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace yaga
{
namespace asset
{

const AssetId Texture::assetId = { "txt", 2 };

// -------------------------------------------------------------------------------------------------------------------------
Texture::Texture(const std::string& name) :
  Asset(name), bytes_(nullptr), width_(0), height_(0), channels_(0), size_(0)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
  stbi_image_free(bytes_);
}

// -------------------------------------------------------------------------------------------------------------------------
size_t Texture::serialize(Asset* asset, std::ostream&, bool)
{
  auto texture = dynamic_cast<Texture*>(asset);
  if (!texture) {
    THROW("Texture serializer was given the wrong asset");
  }
  return 0;
}

// -------------------------------------------------------------------------------------------------------------------------
TexturePtr Texture::deserialize(const std::string& name, std::istream& stream, size_t, bool)
{
  auto texture = std::make_unique<Texture>(name);

  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_ini(stream, props);
  texture->filename_ = props.get<std::string>("file");

  texture->bytes_ = (char*)stbi_load(texture->filename_.c_str(), &texture->width_, &texture->height_, &texture->channels_, STBI_rgb_alpha);
  if (!texture->bytes_) {
    THROW("Could not load texture image");
  }
  texture->size_ = texture->width_ * texture->height_ * 4;

  return std::move(texture);
}

} // !namespace asset
} // !namespace yaga

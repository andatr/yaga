#include "precompiled.h"
#include "assets/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace yaga
{
namespace assets
{

const SerializationInfo Image::serializationInfo = {
  (uint32_t)StandardAssetId::image,
  { "png", "jpg", "jpeg", "bmp", "psd", "tga", "gif", "hdr", "pic" },
  &Image::deserializeBinary,
  &Image::deserializeFriendly
};

// -------------------------------------------------------------------------------------------------------------------------
Image::Image(const std::string& name) :
  Asset(name), bytes_(nullptr), width_(0), height_(0), channels_(0), size_(0)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Image::~Image()
{
  stbi_image_free(bytes_);
}

// -------------------------------------------------------------------------------------------------------------------------
ImagePtr Image::deserializeBinary(const std::string&, std::istream&, size_t, RefResolver&)
{
  THROW_NOT_IMPLEMENTED; 
}

// -------------------------------------------------------------------------------------------------------------------------
ImagePtr Image::deserializeFriendly(const std::string& name, const std::string& path, RefResolver&)
{
  auto image = std::make_unique<Image>(name);
  image->bytes_ = (char*)stbi_load(path.c_str(), &image->width_, &image->height_, &image->channels_, STBI_rgb_alpha);
  if (!image->bytes_) {
    THROW("Could not load image %1%", path);
  }
  image->size_ = image->width_ * image->height_ * 4;
  return image;
}

} // !namespace assets
} // !namespace yaga

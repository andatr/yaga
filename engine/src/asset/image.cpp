#include "precompiled.h"
#include "asset/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace yaga
{
namespace asset
{

const SerializationInfo Image::serializationInfo = {
  3,
  { "png", "jpg", "jpeg", "bmp", "psd", "tga", "gif", "hdr", "pic" },
  "Image",
  &Image::deserialize,
  &Image::deserializeFriendly
};

namespace
{

// -------------------------------------------------------------------------------------------------------------------------
int read(void* user, char* data, int size)
{
  std::istream* stream = static_cast<std::istream*>(user);
  return static_cast<int>(stream->read(data, size).gcount());
}

// -------------------------------------------------------------------------------------------------------------------------
void skip(void* user, int size)
{
  std::istream* stream = static_cast<std::istream*>(user);
  int pos = static_cast<int>(stream->tellg());
  stream->seekg(pos + size, std::ios::cur);
}

// -------------------------------------------------------------------------------------------------------------------------
int eof(void* user)
{
  std::istream* stream = static_cast<std::istream*>(user);
  stream->peek();
  return stream->good() ? 0 : 1;
}

} // !namespace

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
ImagePtr Image::deserialize(const std::string&, std::istream&, size_t)
{
  THROW_NOT_IMPLEMENTED; 
}

// -------------------------------------------------------------------------------------------------------------------------
ImagePtr Image::deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t)
{
  stbi_io_callbacks callbacks;
  callbacks.read = &read;
  callbacks.skip = &skip;
  callbacks.eof = &eof;
  auto image = std::make_unique<Image>(name);
  image->bytes_ = (char*)stbi_load_from_callbacks(&callbacks, &stream, &image->width_, &image->height_, &image->channels_, STBI_rgb_alpha);
  if (!image->bytes_) {
    THROW("Could not load image %1%.%2%", name, type);
  }
  image->size_ = image->width_ * image->height_ * 4;
  return image;
}

} // !namespace asset
} // !namespace yaga

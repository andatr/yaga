#include "precompiled.h"
#include "assets/image.h"
#include "binary_serializer_helper.h"
#include "binary_serializer_registry.h"
#include "friendly_serializer_helper.h"

namespace yaga {
namespace assets {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
int readStbi(void* user, char* data, int size)
{
  auto stream = static_cast<std::istream*>(user);
  return static_cast<int>(stream->read(data, size).gcount());
}

// -----------------------------------------------------------------------------------------------------------------------------
void skipStbi(void* user, int n)
{
  auto stream = static_cast<std::istream*>(user);
  if (n < 0) {
    stream->seekg(-n, std::ios::cur);
  }
  else {
    stream->ignore(n);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
int eofStbi(void* user)
{
  auto stream = static_cast<std::istream*>(user);
  return stream->good() ? 0 : 1;
}

} // !namespace

BINARY_SERIALIZER_REG(Image)

// -----------------------------------------------------------------------------------------------------------------------------
Image::Image(const std::string& name) :
  Asset(name),
  width_(0),
  height_(0),
  format_(0)
{
  addProperty("Bytes",  &bytes_);
  addProperty("Width",  &width_);
  addProperty("Height", &height_);
  addProperty("Format", &format_);
}

// -----------------------------------------------------------------------------------------------------------------------------
Image::~Image()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Image* Image::bytes(Updater handler)
{
  handler(bytes_);
  properties_[PropertyIndex::bytes]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Image* Image::width(uint32_t value)
{
  width_ = value;
  properties_[PropertyIndex::width]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Image* Image::height(uint32_t value)
{
  height_ = value;
  properties_[PropertyIndex::height]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Image* Image::format(int value)
{
  format_ = value;
  properties_[PropertyIndex::format]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
ImagePtr Image::deserializeBinary(std::istream& stream)
{
  std::string name;
  binser::read(stream, name);
  auto image = std::make_unique<Image>(name);
  binser::read(stream, image->width_ );
  binser::read(stream, image->height_);
  binser::read(stream, image->format_);
  binser::read(stream, image->bytes_ );
  return image;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Image::serializeBinary(Asset* asset, std::ostream& stream)
{
  auto image = dynamic_cast<Image*>(asset);
  if (!image) THROW("Image serializer got wrong asset");
  binser::write(stream, image->name_  );
  binser::write(stream, image->width_ );
  binser::write(stream, image->height_); 
  binser::write(stream, image->format_);
  binser::write(stream, image->bytes_ );
}

// -----------------------------------------------------------------------------------------------------------------------------
ImagePtr Image::deserializeFriendly(std::istream& stream)
{
  THROW_NOT_IMPLEMENTED;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Image::serializeFriendly(Asset*, std::ostream&)
{
  THROW_NOT_IMPLEMENTED;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Image::resolveRefs(Asset*, Storage*)
{
}

} // !namespace assets
} // !namespace yaga

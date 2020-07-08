#ifndef YAGA_ASSETS_IMAGE
#define YAGA_ASSETS_IMAGE

#include <istream>
#include <memory>

#include "assets/asset.h"
#include "assets/serializer.h"

namespace yaga {
namespace assets {

class Image;
typedef std::unique_ptr<Image> ImagePtr;

class Image : public Asset
{
public:
  explicit Image(const std::string& name);
  virtual ~Image();
  int width() const { return width_; }
  int height() const { return height_; }
  int channels() const { return channels_; }
  size_t size() const { return size_; }
  const char* data() const { return bytes_; }

public:
  static const SerializationInfo serializationInfo;
  static ImagePtr deserializeBinary(const std::string& name, std::istream& stream, size_t size, RefResolver& resolver);
  static ImagePtr deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver);

private:
  char* bytes_;
  int width_;
  int height_;
  int channels_;
  size_t size_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_IMAGE

#ifndef YAGA_ENGINE_ASSET_IMAGE
#define YAGA_ENGINE_ASSET_IMAGE

#include <memory>
#include <istream>
#include <ostream>

#include "engine/asset/asset.h"
#include "engine/asset/serializer.h"

namespace yaga
{
namespace asset
{

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
  size_t Size() const { return size_; }
  const char* data() const { return bytes_; }
public:
  static const SerializationInfo serializationInfo;
  static ImagePtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static ImagePtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
private:
  char* bytes_;
  int width_;
  int height_;
  int channels_;
  size_t size_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_IMAGE

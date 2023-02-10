#ifndef YAGA_ASSETS_IMAGE
#define YAGA_ASSETS_IMAGE

#include <istream>
#include <memory>
#include <vector>

#include "assets/asset.h"
#include "assets/serializer.h"

namespace yaga {
namespace assets {

class Image;
typedef std::shared_ptr<Image> ImagePtr;

class Image : public Asset
{
public:
  typedef std::vector<char> Bytes;
  typedef std::function<void(Bytes&)> Updater;
  struct PropertyIndex
  {
    static const int  bytes = 0;
    static const int  width = 1;
    static const int height = 2;
    static const int format = 3;
  };

public:
  explicit Image(const std::string& name);
  virtual ~Image();
  const Bytes& bytes() const { return bytes_; }
  uint32_t     width() const { return width_;  }
  uint32_t    height() const { return height_; }
  int         format() const { return format_; }
  AssetType     type() const override { return typeId; }
  Image*  bytes(Updater  handler);
  Image*  width(uint32_t value);
  Image* height(uint32_t value);
  Image* format(int      value);

public:
  static const AssetType typeId = (uint32_t)StandardAssetType::image;
  static ImagePtr deserializeBinary  (std::istream& stream);
  static ImagePtr deserializeFriendly(std::istream& stream);
  static void serializeBinary  (Asset* asset, std::ostream& stream);
  static void serializeFriendly(Asset* asset, std::ostream& stream);
  static void resolveRefs(Asset* asset, Storage* storage);

private:
  std::vector<char> bytes_;
  uint32_t width_;
  uint32_t height_;
  int format_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_IMAGE

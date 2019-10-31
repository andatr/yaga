#ifndef YAGA_ASSET_APPLICATION
#define YAGA_ASSET_APPLICATION

#include <memory>
#include <istream>
#include <ostream>

#include "asset.h"
#include "serializer.h"
#include "utility/array.h"

namespace yaga
{
namespace asset
{

class Application;
typedef std::unique_ptr<Application> ApplicationPtr;

class Application : public Asset
{
public:
  explicit Application(const std::string& name);
  virtual ~Application();
  bool Fullscreen() const { return fullscreen_; }
  uint32_t Width() const { return width_; }
  uint32_t Height() const { return height_; }
  std::string Title() const { return title_; }
public: // Serialization
  static const AssetId assetId;
  static size_t Serialize(Asset* asset, std::ostream& stream, bool binary);
  static ApplicationPtr Deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
private:
  bool fullscreen_;
  uint32_t width_;
  uint32_t height_;
  std::string title_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_APPLICATION

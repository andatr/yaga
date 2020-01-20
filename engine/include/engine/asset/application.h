#ifndef YAGA_ENGINE_ASSET_APPLICATION
#define YAGA_ENGINE_ASSET_APPLICATION

#include <memory>
#include <istream>
#include <ostream>

#include "engine/asset/asset.h"
#include "engine/asset/serializer.h"
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
  bool fullscreen() const { return fullscreen_; }
  uint32_t width() const { return width_; }
  uint32_t height() const { return height_; }
  std::string title() const { return title_; }
public:
  static const SerializationInfo serializationInfo;
  static ApplicationPtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static ApplicationPtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
private:
  bool fullscreen_;
  uint32_t width_;
  uint32_t height_;
  std::string title_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_APPLICATION

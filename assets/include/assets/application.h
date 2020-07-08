#ifndef YAGA_ASSETS_APPLICATION
#define YAGA_ASSETS_APPLICATION

#include <istream>
#include <memory>
#include <string>

#include "assets/asset.h"
#include "assets/serializer.h"
#include "utility/array.h"

namespace yaga {
namespace assets {

class Application;
typedef std::unique_ptr<Application> ApplicationPtr;

class Application : public Asset
{
public:
  explicit Application(const std::string& name);
  virtual ~Application();
  const std::string& rendererLibPath() const { return rendererLibPath_; }
  const std::string& gameLibPath() const { return gameLibPath_; }
  bool fullscreen() const { return fullscreen_; }
  uint32_t width() const { return width_; }
  uint32_t height() const { return height_; }
  std::string title() const { return title_; }
  uint32_t maxTextureCount() const { return maxTextureCount_; }
  uint64_t maxImageSize() const { return maxImageSize_; }
  uint32_t maxIndexCount() const { return maxIndexCount_; }
  uint32_t maxVertexCount() const { return maxVertexCount_; }

public:
  static const SerializationInfo serializationInfo;
  static ApplicationPtr deserializeBinary(const std::string& name, std::istream& stream, size_t size, RefResolver& resolver);
  static ApplicationPtr deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver);

private:
  std::string rendererLibPath_;
  std::string gameLibPath_;
  bool fullscreen_;
  uint32_t width_;
  uint32_t height_;
  std::string title_;
  uint32_t maxTextureCount_;
  uint64_t maxImageSize_;
  uint32_t maxIndexCount_;
  uint32_t maxVertexCount_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_APPLICATION

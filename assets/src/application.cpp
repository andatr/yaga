#include "precompiled.h"
#include "assets/application.h"

namespace yaga {
namespace assets {

const SerializationInfo Application::serializationInfo = {
  (uint32_t)StandardAssetId::application,
  { "yapp" },
  &Application::deserializeBinary,
  &Application::deserializeFriendly
};

// -----------------------------------------------------------------------------------------------------------------------------
Application::Application(const std::string& name) :
  Asset(name), fullscreen_(false), width_(0), height_(0), maxTextureCount_(0), maxImageSize_(0), maxIndexCount_(0),
  maxVertexCount_(0)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Application::~Application()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ApplicationPtr Application::deserializeBinary(const std::string&, std::istream&, size_t, RefResolver&)
{
  THROW_NOT_IMPLEMENTED;
}

// -----------------------------------------------------------------------------------------------------------------------------
ApplicationPtr Application::deserializeFriendly(const std::string& name, const std::string& path, RefResolver&)
{
  auto app = std::make_unique<Application>(name);
  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_json(path, props);
  app->platformLibPath_ = props.get<std::string>("rendererLib");
  app->gameLibPath_ = props.get<std::string>("gameLib");
  auto winProps = props.get_child("window");
  app->fullscreen_ = winProps.get<bool>("fullscreen", false);
  app->width_ = winProps.get<uint32_t>("width", 640);
  app->height_ = winProps.get<uint32_t>("height", 480);
  app->title_ = winProps.get<std::string>("title", "YAGA Game");
  auto limitProps = props.get_child("limits");
  app->maxTextureCount_ = limitProps.get<uint32_t>("textureCount", 0);
  app->maxImageSize_ = limitProps.get<uint64_t>("imageSize", 0);
  app->maxIndexCount_ = limitProps.get<uint32_t>("indexCount", 0);
  app->maxVertexCount_ = limitProps.get<uint32_t>("vertexCount", 0);
  return app;
}

} // !namespace assets
} // !namespace yaga

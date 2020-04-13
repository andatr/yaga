#include "precompiled.h"
#include "asset/application.h"

namespace yaga
{
namespace asset
{

const SerializationInfo Application::serializationInfo = {
  1,
  { "yapp" },
  "Application",
  &Application::deserialize,
  &Application::deserializeFriendly
};

// -------------------------------------------------------------------------------------------------------------------------
Application::Application(const std::string& name) : 
  Asset(name), fullscreen_(false), width_(0), height_(0),
  maxTextureCount_(0), maxImageSize_(0), maxIndexCount_(0), maxVertexCount_(0)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Application::~Application()
{
}

// -------------------------------------------------------------------------------------------------------------------------
ApplicationPtr Application::deserialize(const std::string&, std::istream&, size_t)
{
  THROW_NOT_IMPLEMENTED;
}

// -------------------------------------------------------------------------------------------------------------------------
ApplicationPtr Application::deserializeFriendly(const std::string&, const std::string& name, std::istream& stream, size_t)
{
  auto app = std::make_unique<Application>(name);
  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_json(stream, props);
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

} // !namespace asset
} // !namespace yaga


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
  Asset(name), fullscreen_(false), width_(0), height_(0)
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
  pt::read_ini(stream, props);
  app->fullscreen_ = props.get<bool>("fullscreen", false);
  app->width_ = props.get<uint32_t>("width", 640);
  app->height_ = props.get<uint32_t>("height", 480);
  app->title_ = props.get<std::string>("title", "YAGA Game");
  return app;
}

} // !namespace asset
} // !namespace yaga


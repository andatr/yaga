#include "precompiled.h"
#include "application.h"

namespace yaga
{
namespace asset
{

const AssetId Application::assetId = { "cfg", 1 };

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
size_t Application::serialize(Asset* asset, std::ostream& stream, bool)
{
  auto app = dynamic_cast<Application*>(asset);
  if (!app) {
    THROW("Application serializer was given the wrong asset");
  }
  namespace pt = boost::property_tree;  
  std::stringstream ss;
  pt::ptree window;
  window.put<bool>("fullscreen", app->fullscreen_);
  window.put<uint32_t>("width", app->width_);
  window.put<uint32_t>("height", app->height_);
  window.put<std::string>("title", app->title_);
  pt::ptree props;
  props.add_child("window", window);
  pt::write_json(ss, props);
  stream << ss.rdbuf();
  return ss.tellp();
}

// -------------------------------------------------------------------------------------------------------------------------
ApplicationPtr Application::deserialize(const std::string& name, std::istream& stream, size_t, bool)
{
  auto app = std::make_unique<Application>(name);
  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_json(stream, props);
  auto window = props.get_child("window");
  app->fullscreen_ = window.get<bool>("fullscreen", false);
  app->width_ = window.get<uint32_t>("width", 640);
  app->height_ = window.get<uint32_t>("height", 480);
  app->title_ = window.get<std::string>("title", "YAGA Game");
  return std::move(app);
}

} // !namespace asset
} // !namespace yaga


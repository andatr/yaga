#include "precompiled.h"
#include "engine/config.h"
#include "utility/options.h"
#include "utility/base64.h"

namespace fs = boost::filesystem;

namespace yaga {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
std::string getConfigPath(const ProgramOptions& options)
{
  fs::path path = fs::path(options.getOption<std::string>("cfg", "config.ini"));
  if (!path.is_absolute()) {
    path = fs::path(options.workingDir()) / path;
  }
  if (!fs::is_regular(path)) {
    THROW("Bad configuration path \"%1%\"", path);
  }
  return path.string();
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::ptree readConfigFile(const std::string& path)
{
  boost::property_tree::ptree properties;
  boost::property_tree::ini_parser::read_ini(path, properties);
  return properties;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
void getProperty(const Config::ptree& options, const std::string& name, T& value)
{
  if (options.count(name)) {
    value = options.get<T>(name);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
void setProperty(const Config::ptree& options, const std::string& name, const T& value)
{
  if (options.count(name)) {
    value = options.get<T>(name);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
template <>
inline void getProperty(const Config::ptree& options, const std::string& name, glm::vec4&)
{
  throw std::runtime_error("getProperty<vec4> not implemented");
  // TODO:
  //if (options.count(name)) {
    // value = options.get<T>(name);
  //}
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
boost::program_options::options_description Config::getOptions()
{
  namespace po = boost::program_options;
  po::options_description desc;
  desc.add_options()
    ("cfg,c", po::value<std::string>(), "path to the application config file");
  return desc;
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Config()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Config(const ProgramOptions& options) :
  path_(getConfigPath(options)),
  properties_(readConfigFile(path_)),
  window_   (properties_),
  resources_(properties_),
  rendering_(properties_),
  gui_      (properties_)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::~Config()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::save()
{
  window_.save   (properties_);
  resources_.save(properties_);
  rendering_.save(properties_);
  gui_.save      (properties_);
  boost::property_tree::ini_parser::write_ini(path_, properties_);
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Window::Window() :
  fullscreen_(false),
  width_(640),
  height_(480),
  title_("YAGA")
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Window::Window(const ptree& properties) :
  Window()
{
  if (!properties.count("window")) return;
  const auto& props = properties.get_child("window");
  getProperty(props, "fullscreen", fullscreen_);
  getProperty(props, "width",      width_     );
  getProperty(props, "height",     height_    );
  getProperty(props, "title",      title_     );
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Window::save(ptree& properties)
{
  if (!properties.count("window")) return;
  auto& props = properties.get_child("window");
  props.put("fullscreen", fullscreen_ );
  props.put("width",      width_      );
  props.put("height",     height_     );
  props.put("title",      title_      );
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Window::fullscreen(bool value)
{
  fullscreen_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Window::width(uint32_t value)
{
  width_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Window::height(uint32_t value)
{
  height_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Window::title(const std::string& value)
{
  title_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Resources::Resources() :
  maxTextureCount_(0),
  maxImageSize_(0),
  maxIndexCount_(0),
  maxVertexCount_(0)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Resources::Resources(const ptree& properties) :
  Resources()
{
  if (!properties.count("assets")) return;
  const auto& props = properties.get_child("assets");
  getProperty(props, "maxTextureCount", maxTextureCount_);
  getProperty(props, "maxImageSize",    maxImageSize_   );
  getProperty(props, "maxIndexCount",   maxIndexCount_  );
  getProperty(props, "maxVertexCount",  maxVertexCount_ );
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Resources::save(ptree&)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Resources::maxTextureCount(uint32_t value)
{
  maxTextureCount_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Resources::maxImageSize(uint64_t value)
{
  maxImageSize_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Resources::maxIndexCount(uint32_t value)
{
  maxIndexCount_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Resources::maxVertexCount(uint32_t value)
{
  maxVertexCount_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Rendering::Rendering() :
  msaaLevel_(1),
  depthBuffer_(true),
  clearColor_{ 0.4f, 0.4f, 0.4f, 1.0f },
  clearDepth_(1.0f),
  clearStencil_(0)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Rendering::Rendering(const ptree& properties) :
  Rendering()
{
  if (!properties.count("render")) return;
  const auto& props = properties.get_child("render");
  getProperty(props, "msaaLevel",    msaaLevel_   );
  getProperty(props, "depthBuffer",  depthBuffer_ );
  getProperty(props, "clearColor",   clearColor_  );
  getProperty(props, "clearDepth",   clearDepth_  );
  getProperty(props, "clearStencil", clearStencil_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Rendering::save(ptree& properties)
{
  if (!properties.count("render")) return;
  auto& props = properties.get_child("render");
  props.put("msaaLevel",    msaaLevel_   );
  props.put("depthBuffer",  depthBuffer_ );
  props.put("clearColor",   depthBuffer_ );
  props.put("clearDepth",   clearDepth_  );
  props.put("clearStencil", clearStencil_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Rendering::msaaLevel(int value)
{
  msaaLevel_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Rendering::depthBuffer(bool value)
{
  depthBuffer_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Rendering::clearColor(const glm::vec4& value)
{
  clearColor_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Rendering::clearDepth(float value)
{
  clearDepth_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Rendering::clearStencil(uint32_t value)
{
  clearStencil_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Gui::Gui() :
  clearColor_{ 0.7f, 0.7f, 0.7f, 1.0f }
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Gui::Gui(const ptree& properties) :
  Gui()
{
  if (!properties.count("gui")) return;
  const auto& props = properties.get_child("gui");
  //getProperty(props, "clearColor", clearColor_);
  getProperty(props, "imgui",   imGuiSettings_);
  imGuiSettings_ = base64::decode(imGuiSettings_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Gui::save(ptree& properties)
{
  ptree& props = properties.count("gui")
    ? properties.get_child("gui")
    : properties.add_child("gui", {});
  props.put("imgui", base64::encode(imGuiSettings_));
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Logging::Logging() :
  severity_(log::Severity::info)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::Logging::Logging(const ptree& properties):
  severity_(log::Severity::info)
{
  if (!properties.count("logging")) return;
  const auto& props = properties.get_child("logging");
  std::string severity;
  getProperty(props, "severity", severity);
  severity_ = log::severityFromString(severity);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Config::Logging::save(ptree&)
{
}

} // !namespace yaga

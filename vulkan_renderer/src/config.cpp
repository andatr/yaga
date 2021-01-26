#include "precompiled.h"
#include "config.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Config::Config(const boost::property_tree::ptree& options) :
  fullscreen_(false),
  width_(0),
  height_(0),
  maxTextureCount_(0),
  maxImageSize_(0),
  maxIndexCount_(0),
  maxVertexCount_(0)
{
  auto winProps    = options.get_child("window");
  fullscreen_      = winProps.get<bool>("fullscreen", false);
  width_           = winProps.get<uint32_t>("width", 640);
  height_          = winProps.get<uint32_t>("height", 480);
  title_           = winProps.get<std::string>("title", "YAGA Game");
  auto assetsProps  = options.get_child("assets");
  maxTextureCount_ = assetsProps.get<uint32_t>("maxTextureCount");
  maxImageSize_    = assetsProps.get<uint64_t>("maxImageSize");
  maxIndexCount_   = assetsProps.get<uint32_t>("maxIndexCount");
  maxVertexCount_  = assetsProps.get<uint32_t>("maxVertexCount");

  namespace fs = boost::filesystem;
  guiConfig_ = (fs::path(options.get<std::string>("workingDir")) / "imgui.ini").string();
}

// -----------------------------------------------------------------------------------------------------------------------------
Config::~Config()
{
}

} // !namespace vk
} // !namespace yaga

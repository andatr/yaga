#include "precompiled.h"
#include "basic_application.h"
#include "assets/binary_serializer.h"
#include "assets/friendly_serializer.h"

namespace yaga {
namespace {

// ------------------------------------------------------------------------------------------------------------------------------
assets::SerializerPtr createSerializer(const boost::property_tree::ptree& options)
{
  namespace fs = boost::filesystem;
  auto assetPath = fs::path(options.get<std::string>("assets.path"));
  if (!assetPath.is_absolute()) {
    assetPath = fs::path(options.get<std::string>("workingDir")) / assetPath;
  }
  if (fs::exists(assetPath)) {
    if (fs::is_directory(assetPath)) {
      return std::make_unique<assets::FriendlySerializer>(assetPath.string());
    }
    if (fs::is_regular(assetPath)) {
      return std::make_unique<assets::BinarySerializer>(assetPath.string());
    }
  }
  THROW("Bad asset path \"%1%\"", assetPath);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
BasicApplication::BasicApplication(const boost::property_tree::ptree& options) :
  assets_(std::make_unique<assets::Storage>()),
  context_(nullptr),
  input_(nullptr)
{
  serializer_ = createSerializer(options);
  serializer_->registerStandardAssets();
}

// -----------------------------------------------------------------------------------------------------------------------------
BasicApplication::~BasicApplication()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::init(Context* context, Input* input)
{
  context_ = context;
  input_ = input;
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::resize()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
bool BasicApplication::loop()
{
  return false;
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::shutdown() 
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::gui()
{
}

} // !namespace yaga

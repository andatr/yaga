#include "precompiled.h"
#include "application.h"
#include "vulkan_renderer.h"
#include "utility/main.h"

namespace po = boost::program_options;

namespace yaga {
namespace editor {

struct Options
{
  static po::options_description description();
};

// ------------------------------------------------------------------------------------------------------------------------------
po::options_description Options::description()
{
  po::options_description desc("allowed arguments");
  desc.add(Config::getOptions());
  desc.add_options()
    ("open,o", po::value<std::string>(), "path to the project to open");
  return desc;
}

// ------------------------------------------------------------------------------------------------------------------------------
void main(const ProgramOptions& options)
{
  auto config = std::make_shared<Config>(options);
  log::init(config->logging().severity(), log::format::severity | log::format::time);
  auto platform = createPlatform();
  auto app = std::make_shared<Application>(config, options.getOption<std::string>("open", ""));
  platform->run(app);
}

} // !namespace editor
} // !namespace yaga

YAGA_MAIN(yaga::editor::main, yaga::editor::Options)

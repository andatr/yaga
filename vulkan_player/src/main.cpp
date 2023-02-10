#include "precompiled.h"
#include "application.h"
#include "engine/config.h"
#include "utility/main.h"

namespace po = boost::program_options;

namespace yaga {

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
    ("file,f", po::value<std::string>(), "path to the file to open");
  return desc;
}

// ------------------------------------------------------------------------------------------------------------------------------
void main(const ProgramOptions& options)
{
  auto config = std::make_shared<Config>(options);
  log::init(config->logging().severity(), log::format::severity | log::format::time);
  const std::string filename = options.getOption<std::string>("file", "");
  auto app = std::make_unique<Application>(config, filename);
  app->run();
}

} // !namespace yaga

YAGA_MAIN(yaga::main, yaga::Options)

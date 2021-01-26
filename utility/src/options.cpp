#include "precompiled.h"
#include "options.h"
#include "exception.h"

namespace yaga {

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace {

// --------------------------------------------------------------------------------------------
po::options_description makeOptions()
{
  po::options_description desc("allowed arguments");
  desc.add_options()
    ("app,a", po::value<std::string>(), "path to application config");
  return desc;
}

// --------------------------------------------------------------------------------------------
template <typename T>
const T& getOption(const po::variables_map& options, const std::string& name, const T& defaultVal)
{
  if (options.count(name)) {
    return options[name].as<T>();
  }
  return defaultVal;
}

} // !namespace

// --------------------------------------------------------------------------------------------
ProgramOptions::ProgramOptions(int argc, char* argv[])
{
  po::variables_map options;
  po::store(po::command_line_parser(argc, argv).options(makeOptions()).run(), options);
  po::notify(options);

  auto workingDir = fs::path(argv[0]).parent_path();
  auto appConfigPath = fs::path(getOption<std::string>(options, "app", "config.ini"));
  if (!appConfigPath.is_absolute()) {
    appConfigPath = workingDir / appConfigPath;
  }
  if (!fs::is_regular(appConfigPath)) {
    THROW("Bad application configuration path \"%1%\"", appConfigPath);
  }
  workingDir_ = workingDir.string();
  appConfigPath_ = appConfigPath.string();
  boost::property_tree::ini_parser::read_ini(appConfigPath_, config_);
  config_.add<std::string>("workingDir", appConfigPath.parent_path().string());
}

} // !namespace yaga
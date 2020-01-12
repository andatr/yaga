#include "precompiled.h"
#include "options.h"

namespace yaga
{

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace
{

// --------------------------------------------------------------------------------------------
po::options_description makeOptions()
{
  po::options_description desc("allowed arguments");
  desc.add_options()
    ("app,a", po::value<std::string>(), "path to app directory")
    ("log,l", po::value<std::string>(), "log level: trace, debug, info, warning, error, fatal")
  ;
  return desc;
}

// --------------------------------------------------------------------------------------------
template<typename T>
const T& getOption(const po::variables_map& options, const std::string& name, const T& defaultVal)
{
  if (options.count(name)) {
    return options[name].as<T>();
  }
  return defaultVal;
}

} // !namespace

// --------------------------------------------------------------------------------------------
Options::Options(int argc, char *argv[])
{
  po::variables_map options;
  po::store(po::command_line_parser(argc, argv).options(makeOptions()).run(), options);
  po::notify(options);

  auto appDir = fs::path(getOption<std::string>(options, "app", "data"));
  if (appDir.is_absolute()) {
    appDir_ = appDir.string();
  }
  else {
    auto parent = fs::path(argv[0]).parent_path();
    appDir_ = (parent / appDir).string();
  }

  std::string severity = getOption<std::string>(options, "log", "");
  if (!severity.empty()) {
    severity_ = log::severityFromString(severity);
  }
}

} // !namespace yaga
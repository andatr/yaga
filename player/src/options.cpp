#include "precompiled.h"
#include "options.h"

namespace yaga {
namespace player {

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace {

// -----------------------------------------------------------------------------------------------------------------------------
po::options_description makeOptions()
{
  po::options_description desc("allowed arguments");
  desc.add_options()
    ("file,f", po::value<std::string>(), "path to a file to watch")
    ("log,l",  po::value<std::string>(), "logging level (trace, debug, info, warning, error, fatal)");
  return desc;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& getOption(const po::variables_map& options, const std::string& name, const T& defaultVal)
{
  if (options.count(name)) {
    return options[name].as<T>();
  }
  return defaultVal;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
ProgramOptions::ProgramOptions(int argc, char* argv[])
{
  po::variables_map options;
  po::store(po::command_line_parser(argc, argv).options(makeOptions()).run(), options);
  po::notify(options);

  auto workingDir = fs::path(argv[0]).parent_path();
  workingDir_  = workingDir.string();
  filename_    = getOption<std::string>(options, "file", "");
  logSeverity_ = getOption<std::string>(options, "log",  "info");
}

} // !namespace player
} // !namespace yaga
#include "precompiled.h"
#include "utility/options.h"

namespace yaga {

namespace po = boost::program_options;
namespace fs = boost::filesystem;

// -----------------------------------------------------------------------------------------------------------------------------
ProgramOptions::ProgramOptions(int argc, char* argv[], const po::options_description& desc)
{
  po::store(po::command_line_parser(argc, argv).options(desc).run(), options_);
  po::notify(options_);
  auto path = fs::path(argv[0]);
  workingDir_ = path.parent_path().string();
  appName_ = path.filename().string();
}

} // !namespace yaga
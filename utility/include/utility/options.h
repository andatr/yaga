#ifndef YAGA_UTILITY_OPTIONS
#define YAGA_UTILITY_OPTIONS

#include <string>
#include <boost/property_tree/ptree_fwd.hpp>

namespace yaga {

class ProgramOptions
{
public:
  explicit ProgramOptions(int argc, char* argv[]);
  const std::string& workingDir()    const { return workingDir_;    }
  const std::string& appConfigPath() const { return appConfigPath_; }
  const boost::property_tree::ptree& config() const { return config_; }

private:
  std::string workingDir_;
  std::string appConfigPath_;
  boost::property_tree::ptree config_;
};

} // !namespace yaga

#endif // !YAGA_UTILITY_OPTIONS

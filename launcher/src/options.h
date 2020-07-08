#ifndef YAGA_LAUNCHER_SRC_OPTIONS
#define YAGA_LAUNCHER_SRC_OPTIONS

#include <string>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "utility/log.h"

namespace yaga {

class Options
{
public:
  explicit Options(int argc, char* argv[]);
  const boost::filesystem::path& workingDir() const { return workingDir_; }
  const boost::filesystem::path& appPath() const { return appPath_; }
  boost::optional<log::Severity> logSeverity() const { return severity_; }

private:
  boost::filesystem::path workingDir_;
  boost::filesystem::path appPath_;
  boost::optional<log::Severity> severity_;
};

} // !namespace yaga

#endif // !YAGA_LAUNCHER_SRC_OPTIONS

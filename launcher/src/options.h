#ifndef YAGA_LAUNCHER_SRC_OPTIONS
#define YAGA_LAUNCHER_SRC_OPTIONS

#include <string>

#include <boost/optional.hpp>

#include "utility/log.h"

namespace yaga
{

class Options
{
public:
  explicit Options(int argc, char *argv[]);
  const std::string& appDir() const { return appDir_; }
  boost::optional<log::Severity> logSeverity() const { return severity_; }
private:
  std::string appDir_;
  boost::optional<log::Severity> severity_;
};

} // !namespace yaga

#endif // !YAGA_LAUNCHER_SRC_OPTIONS

#ifndef YAGA_UTILITY_OPTIONS
#define YAGA_UTILITY_OPTIONS

#include "utility/compiler.h"

#include <string>

DISABLE_WARNINGS
#include <boost/program_options.hpp>
ENABLE_WARNINGS

#include "utility/exception.h"

namespace yaga {

class ProgramOptions
{
public:
  ProgramOptions(int argc, char* argv[], const boost::program_options::options_description& desc);
  const std::string& workingDir() const { return workingDir_; }
  const std::string& appName() const { return appName_;}
  template <typename T>
  T getOption(const std::string& name) const;
  template <typename T, typename D>
  T getOption(const std::string& name, const D& defaultVal) const;

private:
  std::string workingDir_;
  std::string appName_;
  boost::program_options::variables_map options_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
ProgramOptions createOptions(int argc, char* argv[])
{
  return ProgramOptions(argc, argv, T::description());
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename D>
T ProgramOptions::getOption(const std::string& name, const D& defaultVal) const
{
  if (options_.count(name)) return options_[name].as<T>();
  return defaultVal;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T ProgramOptions::getOption(const std::string& name) const
{
  if (options_.count(name)) return options_[name].as<T>();
  THROW("Missing program option \"%1%\"", name);
}

} // !namespace yaga

#endif // !YAGA_UTILITY_OPTIONS

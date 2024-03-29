#ifndef YAGA_UTILITY_EXCEPTION
#define YAGA_UTILITY_EXCEPTION

#include "utility/compiler.h"

#include <exception>

DISABLE_WARNINGS
#include <boost/format.hpp>
ENABLE_WARNINGS

#include "utility/log.h"

#define THROW(format, ...) throw yaga::Exception(__FILE__, __LINE__, format, __VA_ARGS__)

#define THROW_NOT_IMPLEMENTED throw yaga::Exception(__FILE__, __LINE__, "%1% not implemented", __func__)

#define LOG_E(severity, exception) BOOST_LOG_TRIVIAL(severity)    \
  << boost::log::add_value(yaga::log::a_file, (exception).File()) \
  << boost::log::add_value(yaga::log::a_line, (exception).Line()) \
  << (exception).what()

namespace yaga {

class Exception : public std::exception
{
public:
  template <typename... Args>
  explicit Exception(const std::string& file, int line, const std::string& format, Args... args);
  const std::string& File() const { return file_; }
  int Line() const { return line_; }
  const char* what() const override { return message_.c_str(); }

private:
  const std::string file_;
  int line_;
  const std::string message_;
};

namespace exception_detail {

// -----------------------------------------------------------------------------------------------------------------------------
inline std::string expandFormat(boost::format f)
{
  return f.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename... Args>
std::string expandFormat(boost::format f, T&& t, Args&&... args)
{
  return expandFormat(f % std::forward<T>(t), std::forward<Args>(args)...);
}

} // !namespace exception_detail

// -----------------------------------------------------------------------------------------------------------------------------
template <typename... Args>
Exception::Exception(const std::string& file, int line, const std::string& format, Args... args) :
  file_(file),
  line_(line),
  message_(exception_detail::expandFormat(boost::format(format), args...))
{
  LOG(error) << message_;
}

} // !namespace yaga

#endif // !YAGA_UTILITY_EXCEPTION
#ifndef YAGA_UTILITY_EXCEPTION
#define YAGA_UTILITY_EXCEPTION

#include <exception>

#include <boost/format.hpp>

#define THROW(format, ...) throw yaga::Exception(__FILE__, __LINE__, format, __VA_ARGS__)

namespace yaga
{

class Exception : public std::exception
{
public:
  template<typename ...Args>
  explicit Exception(const std::string& file, int line, const std::string& format, Args... args);
  const std::string& File() const { return file_; }
  int Line() const { return line_; }
  const char* what() const override { return message_.c_str(); }
private:
  const std::string file_;
  int line_;
  const std::string message_;
};

namespace impl
{

// -------------------------------------------------------------------------------------------------------------------------
inline std::string expandFormat(boost::format f)
{
  return f.str();
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T, typename... Args>
std::string expandFormat(boost::format f, T&& t, Args&&... args)
{
  return expandFormat(f % std::forward<T>(t), std::forward<Args>(args)...);
}

} // !namespace impl

// -------------------------------------------------------------------------------------------------------------------------
template<typename ...Args>
Exception::Exception(const std::string& file, int line, const std::string& format, Args... args) :
  file_(file), line_(line), message_(impl::expandFormat(boost::format(format), args...))
{
}

} // !namespace yaga

#endif // !YAGA_UTILITY_EXCEPTION
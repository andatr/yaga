#ifndef YAGA_RENDERER_APPLICATION
#define YAGA_RENDERER_APPLICATION

#include <memory>
#include <string>

#include <boost/core/noncopyable.hpp>

namespace yaga
{

class Application : private boost::noncopyable
{
public:
  virtual ~Application() {}
  virtual void Run(const std::string& dir) = 0;
};

typedef std::unique_ptr<Application> ApplicationPtr;

ApplicationPtr CreateApplication();

} // !namespace yaga

#endif // !YAGA_RENDERER_APPLICATION

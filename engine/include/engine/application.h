#ifndef YAGA_ENGINE_APPLICATION
#define YAGA_ENGINE_APPLICATION

#include <memory>
#include <string>

#include <boost/core/noncopyable.hpp>

#include "asset/database.h"

namespace yaga
{

class Application : private boost::noncopyable
{
public:
  Application();
  virtual ~Application();
  virtual void run() = 0;
  asset::Database* assets() const { return assets_.get(); }
protected:
  asset::DatabasePtr assets_;
};

typedef std::unique_ptr<Application> ApplicationPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_APPLICATION

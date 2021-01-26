#ifndef YAGA_ENGINE_PLATFORM
#define YAGA_ENGINE_PLATFORM

#include <map>
#include <memory>
#include <string>
#include <boost/noncopyable.hpp>

#include "engine/application.h"
#include "engine/input.h"
#include "engine/context.h"

namespace yaga {

class Platform : private boost::noncopyable
{
public:
  virtual ~Platform() {};
  virtual void run(Application* app) = 0;
};

typedef std::unique_ptr<Platform> PlatformPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_PLATFORM

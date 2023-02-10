#ifndef YAGA_ENGINE_PLATFORM
#define YAGA_ENGINE_PLATFORM

#include "utility/compiler.h"

#include <map>
#include <memory>
#include <string>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "engine/application.h"
#include "engine/input.h"
#include "engine/context.h"

namespace yaga {

class Platform : private boost::noncopyable
{
public:
  virtual ~Platform() {};
  virtual void run(ApplicationPtr app) = 0;
};

typedef std::unique_ptr<Platform> PlatformPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_PLATFORM

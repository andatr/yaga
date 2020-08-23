#ifndef YAGA_ENGINE_PLATFORM
#define YAGA_ENGINE_PLATFORM

#include <map>
#include <memory>
#include <string>
#include <boost/noncopyable.hpp>

#include "assets/application.h"
#include "engine/application.h"
#include "engine/input.h"
#include "engine/rendering_context.h"

namespace yaga {

class Platform : private boost::noncopyable
{
public:
  virtual ~Platform() {};
  virtual void run(Application* app) = 0;
  virtual RenderingContext* renderingContext() = 0;
  virtual Input* input() = 0;
};

typedef std::unique_ptr<Platform> PlatformPtr;
typedef PlatformPtr CreatePlatformFunc(assets::Application*);
constexpr const char* createPlatformFuncName = "createPlatform";

} // !namespace yaga

#endif // !YAGA_ENGINE_PLATFORM

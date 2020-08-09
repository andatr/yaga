#ifndef YAGA_ENGINE_APPLICATION
#define YAGA_ENGINE_APPLICATION

#include <map>
#include <memory>
#include <string>
#include <boost/noncopyable.hpp>

#include "assets/application.h"
#include "engine/game.h"
#include "engine/input.h"
#include "engine/rendering_context.h"

namespace yaga {

class Application : private boost::noncopyable
{
public:
  explicit Application(GamePtr game);
  virtual ~Application();
  virtual void run() = 0;
  virtual RenderingContext* renderingContext() = 0;
  virtual Input* input() = 0;

protected:
  GamePtr game_;
};

typedef std::unique_ptr<Application> ApplicationPtr;
typedef ApplicationPtr CreateApplicationFunc(GamePtr, assets::Application*);
constexpr const char* createApplicationFuncName = "createApplication";

} // !namespace yaga

#endif // !YAGA_ENGINE_APPLICATION

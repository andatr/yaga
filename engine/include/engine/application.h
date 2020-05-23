#ifndef YAGA_ENGINE_APPLICATION
#define YAGA_ENGINE_APPLICATION

#include <map>
#include <memory>
#include <string>

#include <boost/noncopyable.hpp>

#include "engine/game.h"
#include "engine/rendering_context.h"

namespace yaga
{

class Application : private boost::noncopyable
{
public:
  explicit Application(Game* game);
  virtual ~Application();
  virtual void run() = 0;
  virtual RenderingContext* renderingContext() = 0;
protected:
  void gameInit() { game_->app_ = this; game_->init(); }
  void gameLoop(float delta) const { game_->loop(delta); }
  void gameShutdown() const { game_->shutdown(); }
protected:
  Game* game_;
};

typedef std::unique_ptr<Application> ApplicationPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_APPLICATION

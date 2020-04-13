#ifndef YAGA_ENGINE_APPLICATION
#define YAGA_ENGINE_APPLICATION

#include <map>
#include <memory>
#include <string>

#include <boost/core/noncopyable.hpp>

#include "engine/game.h"
#include "engine/asset/scene.h"

namespace yaga
{

class Application : private boost::noncopyable
{
public:
  explicit Application(GamePtr game);
  virtual ~Application();
  Game* game() const { return game_.get(); }
  virtual Scene* createScene(asset::Scene* asset) = 0;
  virtual void run() = 0;
protected:
  GamePtr game_;
};

typedef std::unique_ptr<Application> ApplicationPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_APPLICATION

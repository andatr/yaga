#ifndef YAGA_ENGINE_GAME
#define YAGA_ENGINE_GAME

#include <memory>

#include "asset/database.h"
#include "engine/scene.h"
#include "engine/scene_clipper.h"

namespace yaga
{

class Application;

class Game
{
public:
  explicit Game() : app_(nullptr) {}
  virtual ~Game() {}
  void application(Application* app) { app_ = app; }
  virtual void init() = 0;
  virtual void loop(float delta) = 0;
  virtual void shutdown() = 0;
  virtual Scene* scene() = 0;
  virtual SceneClipper* sceneClipper() = 0;
  virtual asset::Database* assets() = 0;
protected:
  Application* app_;
};

typedef std::unique_ptr<Game> GamePtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_GAME

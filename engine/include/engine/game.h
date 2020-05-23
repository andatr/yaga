#ifndef YAGA_ENGINE_GAME
#define YAGA_ENGINE_GAME

#include <memory>

#include "asset/database.h"

namespace yaga
{

class Application;

class Game
{
friend class Application;
public:
  explicit Game() : app_(nullptr) {}
  virtual ~Game() {}
private:
  virtual void init() = 0;
  virtual void loop(float delta) = 0;
  virtual void shutdown() = 0;
protected:
  Application* app_;
};

typedef std::unique_ptr<Game> GamePtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_GAME

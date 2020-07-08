#ifndef YAGA_ENGINE_GAME
#define YAGA_ENGINE_GAME

#include <memory>

#include "assets/serializer.h"
#include "assets/storage.h"

namespace yaga {

class Application;

class Game
{
  friend class Application;

public:
  virtual ~Game() {}
  virtual void init(Application* app) = 0;
  virtual void resize() = 0;
  virtual void loop(float delta) = 0;
  virtual void shutdown() = 0;
};

typedef std::unique_ptr<Game> GamePtr;
typedef GamePtr CreateGameFunc(assets::SerializerPtr, assets::StoragePtr);
constexpr const char* createGameFuncName = "createGame";

} // !namespace yaga

#endif // !YAGA_ENGINE_GAME

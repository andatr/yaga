#ifndef YAGA_ENGINE_BASIC_GAME
#define YAGA_ENGINE_BASIC_GAME

#include <memory>

#include <boost/core/noncopyable.hpp>

#include "asset/database.h"
#include "engine/game.h"
#include "engine/scene.h"

namespace yaga
{

class BasicGame : private boost::noncopyable, public Game
{
public:
  BasicGame();
  virtual ~BasicGame();
  void init() override;
  void loop(float delta) override;
  void shutdown() override;
  Scene* scene() override { return scene_; }
  SceneClipper* sceneClipper() override { return sceneClipper_.get(); }
  asset::Database* assets() override { return assets_.get(); }
protected:
  Scene* scene_;
  SceneClipperPtr sceneClipper_;
  asset::DatabasePtr assets_;
};

typedef std::unique_ptr<BasicGame> BasicGamePtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_BASIC_GAME

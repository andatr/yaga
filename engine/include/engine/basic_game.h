#ifndef YAGA_ENGINE_BASIC_GAME
#define YAGA_ENGINE_BASIC_GAME

#include <memory>

#include <boost/noncopyable.hpp>

#include "assets/serializer.h"
#include "assets/storage.h"
#include "engine/game.h"

namespace yaga
{

class BasicGame : private boost::noncopyable, public Game
{
public:
  BasicGame(assets::SerializerPtr serializer, assets::StoragePtr storage);
  virtual ~BasicGame();
  assets::Storage* persistentAssets() const { return persistentAssets_.get(); }
protected:
  void init(Application* app) override;
  void resize() override;
  void loop(float delta) override;
  void shutdown() override;
protected:
  Application* app_;
  assets::SerializerPtr serializer_;
  assets::StoragePtr persistentAssets_;
};

typedef std::unique_ptr<BasicGame> BasicGamePtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_BASIC_GAME

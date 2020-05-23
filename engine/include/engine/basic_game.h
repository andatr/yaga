#ifndef YAGA_ENGINE_BASIC_GAME
#define YAGA_ENGINE_BASIC_GAME

#include <memory>

#include <boost/noncopyable.hpp>

#include "asset/database.h"
#include "engine/camera.h"
#include "engine/game.h"
#include "engine/object.h"

namespace yaga
{

class BasicGame : private boost::noncopyable, public Game
{
public:
  BasicGame();
  virtual ~BasicGame();
  asset::Database* assets() const { return assets_.get(); }
private:
  void init() override;
  void loop(float delta) override;
  void shutdown() override;
protected:
  asset::DatabasePtr assets_;
  std::vector<ObjectPtr> objects_;
  Camera* camera_;
  Transform* cameraPosition_;
  Transform* objectPosition_;
};

typedef std::unique_ptr<BasicGame> BasicGamePtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_BASIC_GAME

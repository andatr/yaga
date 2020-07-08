#ifndef YAGA_TUBE_RACE_SRC_RACE_GAME
#define YAGA_TUBE_RACE_SRC_RACE_GAME

#include <memory>
#include <boost/dll/alias.hpp>

#include "engine/basic_game.h"

namespace yaga {

class TubeRace : public BasicGame
{
public:
  TubeRace(assets::SerializerPtr serializer, assets::StoragePtr storage);
  virtual ~TubeRace();

protected:
  void init(Application* app) override;
  void loop(float delta) override;
  void shutdown() override;

private:
  typedef BasicGame base;
};

typedef std::unique_ptr<TubeRace> TubeRacePtr;

GamePtr createGame(assets::SerializerPtr serializer, assets::StoragePtr storage);
BOOST_DLL_ALIAS(yaga::createGame, createGame)

} // !namespace yaga

#endif // !YAGA_TUBE_RACE_SRC_RACE_GAME

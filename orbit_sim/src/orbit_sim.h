#ifndef YAGA_ORBIT_SIM_SRC_ORBIT_SIM
#define YAGA_ORBIT_SIM_SRC_ORBIT_SIM

#include <memory>
#include <boost/dll/alias.hpp>

#include "engine/basic_game.h"

namespace yaga {

class OrbitSim : public BasicGame
{
public:
  OrbitSim(assets::SerializerPtr serializer, assets::StoragePtr storage);
  virtual ~OrbitSim();

private:
  void init(Application* app) override;
  void loop(float delta) override;
  void shutdown() override;

private:
  typedef BasicGame base;
};

typedef std::unique_ptr<OrbitSim> OrbitSimPtr;

GamePtr createGame(assets::SerializerPtr serializer, assets::StoragePtr storage);
BOOST_DLL_ALIAS(yaga::createGame, createGame)

} // !namespace yaga

#endif // !YAGA_ORBIT_SIM_SRC_ORBIT_SIM

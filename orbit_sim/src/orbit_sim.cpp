#include "precompiled.h"
#include "orbit_sim.h"

#include "engine/application.h"
#include "assets/scene.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
GamePtr createGame(assets::SerializerPtr serializer, assets::StoragePtr storage)
{
  return std::make_unique<OrbitSim>(std::move(serializer), std::move(storage));
}

// -------------------------------------------------------------------------------------------------------------------------
OrbitSim::OrbitSim(assets::SerializerPtr serializer, assets::StoragePtr storage) :
  BasicGame(std::move(serializer), std::move(storage))
{
}

// -------------------------------------------------------------------------------------------------------------------------
OrbitSim::~OrbitSim()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void OrbitSim::init(Application* app)
{
  base::init(app);
}

// -------------------------------------------------------------------------------------------------------------------------
void OrbitSim::loop(float delta)
{
  base::loop(delta);
}

// -------------------------------------------------------------------------------------------------------------------------
void OrbitSim::shutdown()
{
  base::shutdown();
}

} // !namespace yaga


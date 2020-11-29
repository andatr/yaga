#include "precompiled.h"
#include "orbit_sim.h"
#include "assets/scene.h"
#include "engine/platform.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
ApplicationPtr createApplication(assets::Serializer* serializer)
{
  return std::make_unique<OrbitSim>(serializer);
}

// -----------------------------------------------------------------------------------------------------------------------------
OrbitSim::OrbitSim(assets::Serializer* serializer) :
  BasicApplication(serializer)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
OrbitSim::~OrbitSim()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void OrbitSim::init(RenderingContext* renderer, Input* input)
{
  base::init(renderer, input);
}

// -----------------------------------------------------------------------------------------------------------------------------
void OrbitSim::loop(float delta)
{
  base::loop(delta);
}

// -----------------------------------------------------------------------------------------------------------------------------
void OrbitSim::shutdown()
{
  base::shutdown();
}

} // !namespace yaga

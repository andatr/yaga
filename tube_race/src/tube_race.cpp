#include "precompiled.h"
#include "tube_race.h"
#include "assets/scene.h"
#include "engine/platform.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
ApplicationPtr createApplication(assets::Serializer* serializer)
{
  return std::make_unique<TubeRace>(serializer);
}

// -----------------------------------------------------------------------------------------------------------------------------
TubeRace::TubeRace(assets::Serializer* serializer) :
  BasicApplication(serializer)
{}

// -----------------------------------------------------------------------------------------------------------------------------
TubeRace::~TubeRace()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void TubeRace::init(Context* renderer, Input* input)
{
  base::init(renderer, input);
}

// -----------------------------------------------------------------------------------------------------------------------------
bool TubeRace::loop()
{
  if (!base::loop()) return false;
  return false;
}

// -----------------------------------------------------------------------------------------------------------------------------
void TubeRace::shutdown()
{
  base::shutdown();
}

} // !namespace yaga

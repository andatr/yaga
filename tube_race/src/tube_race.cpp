#include "precompiled.h"
#include "tube_race.h"

#include "engine/application.h"
#include "assets/scene.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
GamePtr createGame(assets::SerializerPtr serializer, assets::StoragePtr storage)
{
  return std::make_unique<TubeRace>(std::move(serializer), std::move(storage));
}

// -------------------------------------------------------------------------------------------------------------------------
TubeRace::TubeRace(assets::SerializerPtr serializer, assets::StoragePtr storage) :
  BasicGame(std::move(serializer), std::move(storage))
{
}

// -------------------------------------------------------------------------------------------------------------------------
TubeRace::~TubeRace()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void TubeRace::init(Application* app)
{
  base::init(app);
}

// -------------------------------------------------------------------------------------------------------------------------
void TubeRace::loop(float delta)
{
  base::loop(delta);
}

// -------------------------------------------------------------------------------------------------------------------------
void TubeRace::shutdown()
{
  base::shutdown();
}

} // !namespace yaga


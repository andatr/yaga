#include "precompiled.h"
#include "basic_game.h"

#include "engine/application.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
BasicGame::BasicGame() :
  assets_(std::make_unique<asset::Database>())
{
}
  
// -------------------------------------------------------------------------------------------------------------------------
BasicGame::~BasicGame()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::init()
{
  auto scene_asset = assets_->get<asset::Scene>("scene");
  scene_ = app_->createScene(scene_asset);
  sceneClipper_ = std::make_unique<SceneClipper>(scene_);
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::loop(float delta)
{
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::shutdown()
{
}

} // !namespace yaga


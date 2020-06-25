#include "precompiled.h"
#include "basic_game.h"

#include "engine/application.h"
#include "assets/scene.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
BasicGame::BasicGame(assets::SerializerPtr serializer, assets::StoragePtr storage) :
  app_(nullptr), persistentAssets_(std::move(storage)), serializer_(std::move(serializer))
{
}

// -------------------------------------------------------------------------------------------------------------------------
BasicGame::~BasicGame()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::init(Application* app)
{
  app_ = app;
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::resize()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::loop(float)
{
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::shutdown()
{
}

} // !namespace yaga


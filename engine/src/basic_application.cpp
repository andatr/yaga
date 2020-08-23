#include "precompiled.h"
#include "basic_application.h"
#include "assets/binary_serializer.h"
#include "assets/friendly_serializer.h"
#include "assets/scene.h"
#include "assets/serializer.h"
#include "assets/storage.h"
#include "engine/platform.h"

namespace fs = boost::filesystem;

namespace yaga {

// -------------------------------------------------------------------------------------------------------------------------
BasicApplication::BasicApplication(assets::Serializer* serializer) :
  persistentAssets_(std::make_unique<assets::Storage>()),
  serializer_(serializer),
  renderer_(nullptr),
  input_(nullptr)
{
}

// -------------------------------------------------------------------------------------------------------------------------
BasicApplication::~BasicApplication()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicApplication::init(RenderingContext* renderer, Input* input)
{
  renderer_ = renderer;
  input_ = input;
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicApplication::resize()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicApplication::loop(float)
{
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicApplication::shutdown() 
{
}

} // !namespace yaga

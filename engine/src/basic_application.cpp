#include "precompiled.h"
#include "basic_application.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
BasicApplication::BasicApplication(assets::Serializer* serializer) :
  persistentAssets_(std::make_unique<assets::Storage>()),
  serializer_(serializer),
  context_(nullptr),
  input_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
BasicApplication::~BasicApplication()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::init(Context* context, Input* input)
{
  context_ = context;
  input_ = input;
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::resize()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
bool BasicApplication::loop()
{
  return false;
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::shutdown() 
{
}

} // !namespace yaga

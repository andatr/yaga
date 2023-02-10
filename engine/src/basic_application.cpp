#include "precompiled.h"
#include "engine/basic_application.h"
#include "assets/binary_serializer.h"

namespace yaga {
namespace {

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
BasicApplication::BasicApplication(ConfigPtr config) :
  config_(config),
  running_(false),
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
  running_ = true;
  context_ = context;
  input_ = input;
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::resize()
{
  config_->window().width (context_->resolution().x);
  config_->window().height(context_->resolution().y);
}

// -----------------------------------------------------------------------------------------------------------------------------
bool BasicApplication::loop()
{
  return false;
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::stop()
{
  running_ = false;
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::shutdown()
{
  config_->save();
}

// -----------------------------------------------------------------------------------------------------------------------------
void BasicApplication::gui()
{
}

} // !namespace yaga

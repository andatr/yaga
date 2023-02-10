#include "precompiled.h"
#include "engine/renderer.h"
#include "engine/object.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
Renderer::Renderer() :
  layer_(0)
{
  addProperty("Layer", &layer_);
}

// -----------------------------------------------------------------------------------------------------------------------------
Renderer::~Renderer()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Renderer* Renderer::layer(uint32_t value)
{
  layer_ = value;
  properties_[PropertyIndex::layer]->update(this);
  return this;
}

} // !namespace yaga

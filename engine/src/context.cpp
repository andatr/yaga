#include "precompiled.h"
#include "engine/context.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
Context::Context()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
TransformPtr Context::createTransform(assets::TransformPtr asset)
{
  return std::unique_ptr<Transform>(new Transform(asset));
}

} // !namespace yaga

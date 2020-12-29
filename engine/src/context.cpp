#include "precompiled.h"
#include "context.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
TransformPtr Context::createTransform(Object* object)
{
  return std::make_unique<Transform>(object);
}

// -----------------------------------------------------------------------------------------------------------------------------
CameraPtr Context::createCamera(Object* object)
{
  return std::make_unique<Camera>(object);
}

} // !namespace yaga

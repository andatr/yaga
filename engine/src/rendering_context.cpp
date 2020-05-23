#include "precompiled.h"
#include "rendering_context.h"

namespace yaga
{

TransformPtr RenderingContext::createTransform(Object* object)
{
  return std::make_unique<Transform>(object);
}

} // !namespace yaga


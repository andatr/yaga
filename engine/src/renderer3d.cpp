#include "precompiled.h"
#include "engine/renderer3d.h"

namespace yaga {
namespace {

const std::string componentName = "Renderer3D";

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Renderer3D::Renderer3D()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
const std::string& Renderer3D::name()
{
  return componentName;
}

} // !namespace yaga

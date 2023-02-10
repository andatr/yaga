#include "precompiled.h"
#include "engine/mesh.h"

namespace yaga {
namespace {

const std::string componentName = "Mesh";

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(assets::MeshPtr asset) :
  asset_(asset)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::PropertyList& Mesh::properties()
{
  return asset_->properties();
}

// -----------------------------------------------------------------------------------------------------------------------------
const std::string& Mesh::name()
{
  return componentName;
}

} // !namespace yaga

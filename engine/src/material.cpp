#include "precompiled.h"
#include "engine/material.h"

namespace yaga {
namespace {

const std::string componentName = "Material";

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Material::Material(assets::MaterialPtr asset) :
  asset_(asset)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
const std::string& Material::name()
{
  return componentName;
}

} // !namespace yaga

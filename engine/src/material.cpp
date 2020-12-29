#include "precompiled.h"
#include "material.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
Material::Material(Object* object, assets::Material* asset) :
  Component(object),
  asset_(asset)
{
}

} // !namespace yaga

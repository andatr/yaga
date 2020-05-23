#include "precompiled.h"
#include "material.h"

namespace yaga
{

// ------------------------------------------------------------------------------------------------------------------------
Material::Material(Object* object, asset::Material* asset) :
  Component(object), asset_(asset)
{
}

} // !namespace yaga


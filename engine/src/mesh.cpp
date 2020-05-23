#include "precompiled.h"
#include "mesh.h"

namespace yaga
{

// ------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(Object* object, asset::Mesh* asset) :
  Component(object), asset_(asset)
{
}

} // !namespace yaga


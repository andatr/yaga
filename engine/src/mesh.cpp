#include "precompiled.h"
#include "mesh.h"

namespace yaga {

// ------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(Object* object, assets::Mesh* asset) : Component(object), asset_(asset)
{
}

} // !namespace yaga

#include "precompiled.h"
#include "mesh.h"
#include "mesh_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(MeshPool* pool, 
  Object* object,
  assets::Mesh* asset,
  Buffer* vertices,
  Buffer* indices,
  uint32_t vertexCount,
  uint32_t indexCount) :
    pool_(pool),
    yaga::Mesh(object, asset),
    vertexBuffer_(vertices),
    indexBuffer_(indices),
    vertexCount_(vertexCount),
    indexCount_(indexCount)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
  pool_->onRemove(this);
}

} // !namespace vk
} // !namespace yaga
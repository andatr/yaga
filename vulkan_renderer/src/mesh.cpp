#include "precompiled.h"
#include "mesh.h"
#include "mesh_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(Object* object, assets::Mesh* asset, MeshPool* pool, VkBuffer vertices, VkBuffer indices, uint32_t indexCount) :
  yaga::Mesh(object, asset), pool_(pool), vertexBuffer_(vertices), indexBuffer_(indices), indexCount_(indexCount)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
  pool_->removeMesh(this);
}

} // !namespace vk
} // !namespace yaga
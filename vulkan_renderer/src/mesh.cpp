#include "precompiled.h"
#include "vulkan_renderer/mesh.h"
#include "vulkan_renderer/mesh_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(
  MeshPool* pool, 
  assets::MeshPtr asset,
  Buffer* vertices,
  Buffer* indices
) :
  yaga::Mesh(asset),
  vertexDirty_(true),
  indexDirty_(true),
  pool_(pool),
  vertexBuffer_(vertices),
  indexBuffer_(indices)
{
  connections_.push_back(asset->properties(assets::Mesh::PropertyIndex::vertices)->onUpdate([this](void*) {
    vertexDirty_ = true;
  }));
  connections_.push_back(asset->properties(assets::Mesh::PropertyIndex::indices )->onUpdate([this](void*) {
    indexDirty_ = true;
  }));
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
  pool_->remove(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Mesh::update() {
  if (!vertexDirty_ && !indexDirty_) return;
  pool_->update(this);
  vertexDirty_ = false;
  indexDirty_  = false;
}

} // !namespace vk
} // !namespace yaga
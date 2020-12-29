#include "precompiled.h"
#include "renderer_3d_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Renderer3DPool::Renderer3DPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Renderer3DPool::~Renderer3DPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Renderer3DPtr Renderer3DPool::create(Object* object)
{
  auto renderer3d = std::make_unique<Renderer3D>(this, object);
  auto ptr = renderer3d.get();
  renderers3D_.insert(ptr);
  return std::move(renderer3d);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Renderer3DPool::onRemove(Renderer3D* renderer)
{
  renderers3D_.erase(renderer);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Renderer3DPool::clear()
{
  if (!renderers3D_.empty()) {
    THROW("Not all 3D renderes were returned to the pool");
  }
}

} // !namespace vk
} // !namespace yaga
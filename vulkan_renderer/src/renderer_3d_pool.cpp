#include "precompiled.h"
#include "vulkan_renderer/renderer_3d_pool.h"

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
Renderer3DPtr Renderer3DPool::create()
{
  auto renderer3d = std::make_unique<Renderer3D>(this);
  auto ptr = renderer3d.get();
  renderers3D_.insert(ptr);
  return std::move(renderer3d);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Renderer3DPool::remove(Renderer3D* renderer)
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
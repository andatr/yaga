#ifndef YAGA_VULKAN_RENDERER_SRC_RENDERER_3D_POOL
#define YAGA_VULKAN_RENDERER_SRC_RENDERER_3D_POOL

#include <memory>
#include <unordered_set>

#include "vulkan.h"
#include "renderer3d.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Renderer3DPool
{
public:
  explicit Renderer3DPool();
  ~Renderer3DPool();
  Renderer3DPtr create(Object* object);
  void onRemove(Renderer3D* renderer);
  void clear();
  const std::unordered_set<Renderer3D*>& all() const { return renderers3D_; }

private:
  std::unordered_set<Renderer3D*> renderers3D_;
};

typedef std::unique_ptr<Renderer3DPool> Renderer3DPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDERER_3D_POOL

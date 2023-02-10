#ifndef YAGA_VULKAN_RENDERER_MESH
#define YAGA_VULKAN_RENDERER_MESH

#include <memory>

#include "vulkan_renderer/buffer.h"
#include "vulkan_renderer/vulkan.h"
#include "engine/mesh.h"
#include "utility/signal.h"

namespace yaga {
namespace vk {

class MeshPool;

class Mesh : public yaga::Mesh
{
public:
  Mesh(
    MeshPool* pool,
    assets::MeshPtr asset,
    Buffer* vertices,
    Buffer* indices);
  virtual ~Mesh();
  void update();
  Buffer*  vertexBuffer() const { return vertexBuffer_; }
  Buffer*   indexBuffer() const { return indexBuffer_;  }
  bool      vertexDirty() const { return vertexDirty_;  }
  bool       indexDirty() const { return indexDirty_;   }

private:
  bool      vertexDirty_;
  bool      indexDirty_;
  MeshPool* pool_;
  Buffer*   vertexBuffer_;
  Buffer*   indexBuffer_;
  SignalConnections connections_;
};

typedef std::unique_ptr<Mesh> MeshPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_MESH

#ifndef YAGA_VULKAN_RENDERER_SRC_MESH
#define YAGA_VULKAN_RENDERER_SRC_MESH

#include <memory>

#include "buffer.h"
#include "vulkan.h"
#include "engine/mesh.h"

namespace yaga {
namespace vk {

class MeshPool;

// -----------------------------------------------------------------------------------------------------------------------------
class Mesh : public yaga::Mesh
{
public:
  explicit Mesh(MeshPool* pool,
    Object* object,
    assets::Mesh* asset,
    Buffer* vertices,
    Buffer* indices,
    uint32_t vertexCount,
    uint32_t indexCount);
  virtual ~Mesh();
  Buffer*  vertexBuffer() const { return vertexBuffer_; }
  Buffer*   indexBuffer() const { return indexBuffer_;  }
  uint32_t  vertexCount() const { return vertexCount_;  }
  uint32_t   indexCount() const { return indexCount_;   }

private:
  MeshPool* pool_;
  Buffer*   vertexBuffer_;
  Buffer*   indexBuffer_;
  uint32_t  vertexCount_;
  uint32_t  indexCount_;
};

typedef std::unique_ptr<Mesh> MeshPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_MESH

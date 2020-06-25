#ifndef YAGA_VULKAN_RENDERER_SRC_MESH
#define YAGA_VULKAN_RENDERER_SRC_MESH

#include <memory>

#include "buffer.h"
#include "vulkan.h"
#include "engine/mesh.h"

namespace yaga
{
namespace vk
{
// -------------------------------------------------------------------------------------------------------------------------
class Mesh : public yaga::Mesh
{
friend class MeshPool;
public:
  explicit Mesh(Object* object, assets::Mesh* asset, MeshPool* pool, VkBuffer vertices, VkBuffer indices, uint32_t indexCount);
  virtual ~Mesh();
  VkBuffer vertexBuffer() const { return vertexBuffer_; }
  VkBuffer indexBuffer() const { return indexBuffer_; }
  uint32_t indexCount() const { return indexCount_; }
private:
  MeshPool* pool_;
  VkBuffer vertexBuffer_;
  VkBuffer indexBuffer_;
  uint32_t indexCount_;
};

typedef std::unique_ptr<Mesh> MeshPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_MESH

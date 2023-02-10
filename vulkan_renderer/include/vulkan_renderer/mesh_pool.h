#ifndef YAGA_VULKAN_RENDERER_MESH_POOL
#define YAGA_VULKAN_RENDERER_MESH_POOL

#include <memory>
#include <unordered_map>

#include "vulkan_renderer/buffer.h"
#include "vulkan_renderer/device.h"
#include "vulkan_renderer/mesh.h"
#include "vulkan_renderer/vulkan.h"
#include "assets/mesh.h"
#include "engine/config.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class MeshPool
{
public:
  explicit MeshPool(Device* device, VmaAllocator allocator, const Config::Resources& config);
  ~MeshPool();
  MeshPtr get(assets::MeshPtr asset);
  void clear();
  void remove(Mesh* mesh);
  void update(Mesh* mesh);

private:
  struct MeshCache
  {
    BufferPtr vertices;
    BufferPtr indices;
  };

private:
  void createStageBuffers(uint32_t maxVertexCount, uint32_t maxIndexCount);

private:
  size_t counter_;
  Device* device_;
  VmaAllocator allocator_;
  uint32_t maxVertexCount_;
  uint32_t maxIndexCount_;
  BufferPtr stageVertexBuffer_;
  BufferPtr stageIndexBuffer_;
  std::unordered_map<assets::MeshPtr, MeshCache> meshes_;
};

typedef std::unique_ptr<MeshPool> MeshPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_MESH_POOL

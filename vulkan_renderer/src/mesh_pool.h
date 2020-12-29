#ifndef YAGA_VULKAN_RENDERER_SRC_MESH_POOL
#define YAGA_VULKAN_RENDERER_SRC_MESH_POOL

#include <memory>
#include <unordered_map>

#include "buffer.h"
#include "device.h"
#include "mesh.h"
#include "vulkan.h"
#include "assets/application.h"
#include "assets/mesh.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class MeshPool
{
public:
  explicit MeshPool(Device* device, VmaAllocator allocator, const assets::Application* limits);
  ~MeshPool();
  MeshPtr get(Object* object, assets::Mesh* asset);
  void clear();
  void onRemove(Mesh* mesh);

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
  std::unordered_map<assets::Mesh*, MeshCache> meshes_;
};

typedef std::unique_ptr<MeshPool> MeshPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_MESH_POOL

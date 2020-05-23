#ifndef YAGA_VULKAN_RENDERER_SRC_MESH_POOL
#define YAGA_VULKAN_RENDERER_SRC_MESH_POOL

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "buffer.h"
#include "device.h"
#include "mesh.h"
#include "vulkan.h"
#include "engine/asset/mesh.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class MeshPool
{
public:
  explicit MeshPool(Device* device, VmaAllocator allocator, uint32_t maxVertexCount, uint32_t maxIndexCount);
  MeshPtr createMesh(Object* object, asset::Mesh* asset);
  void clear();
  void removeMesh(Mesh* mesh);
private:
  void createStageBuffers(uint32_t maxVertexCount, uint32_t maxIndexCount);
private:
  struct MeshCache
  {
    uint32_t indexCount;
    BufferPtr vertexBuffer;
    BufferPtr indexBuffer;
  };
private:
  Device* device_;
  VkDevice vkDevice_;
  VmaAllocator allocator_;
  uint32_t maxVertexCount_;
  uint32_t maxIndexCount_;
  BufferPtr stageVertexBuffer_;
  BufferPtr stageIndexBuffer_;
  std::unordered_map<asset::Mesh*, MeshCache> meshCache_;
  std::unordered_set<Mesh*> meshes_;
};

typedef std::unique_ptr<MeshPool> MeshPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_MESH_POOL

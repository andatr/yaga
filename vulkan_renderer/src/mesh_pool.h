#ifndef YAGA_VULKAN_RENDERER_SRC_MESH_POOL
#define YAGA_VULKAN_RENDERER_SRC_MESH_POOL

#include <map>
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
  Mesh* createMesh(asset::Mesh* asset);
  void clear() { meshes_.clear(); }
private:
  void createStageBuffers(uint32_t maxVertexCount, uint32_t maxIndexCount);
private:
  Device* device_;
  VkDevice vkDevice_;
  VmaAllocator allocator_;
  uint32_t maxVertexCount_;
  uint32_t maxIndexCount_;
  BufferPtr stageVertexBuffer_;
  BufferPtr stageIndexBuffer_;
  std::map<asset::Mesh*, MeshPtr> meshes_;
};

typedef std::unique_ptr<MeshPool> MeshPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_MESH_POOL

#ifndef YAGA_VULKAN_RENDERER_GUI_MESH_POOL
#define YAGA_VULKAN_RENDERER_GUI_MESH_POOL

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

class GuiMeshPool
{
public:
  explicit GuiMeshPool(Device* device, VmaAllocator allocator, const Config::Resources& config);
  ~GuiMeshPool();

private:
  size_t counter_;
  Device* device_;
  VmaAllocator allocator_;
  uint32_t maxVertexCount_;
  uint32_t maxIndexCount_;
  BufferPtr stageVertexBuffer_;
  BufferPtr stageIndexBuffer_;
};

typedef std::unique_ptr<MeshPool> MeshPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_GUI_MESH_POOL

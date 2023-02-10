#include "precompiled.h"
#include "vulkan_renderer/gui_mesh_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
GuiMeshPool::GuiMeshPool(Device* device, VmaAllocator allocator, const Config::Resources& config) :
  counter_(0),
  device_(device),
  allocator_(allocator),
  maxVertexCount_(config.maxVertexCount()),
  maxIndexCount_(config.maxIndexCount())
{
}

// -----------------------------------------------------------------------------------------------------------------------------
GuiMeshPool::~GuiMeshPool()
{
}

} // !namespace vk
} // !namespace yaga
#include "precompiled.h"
#include "vulkan_renderer/mesh.h"
#include "vulkan_renderer/post_processor_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
PostProcessor::PostProcessor(PostProcessorPool* pool, int order, bool hostMemory, Proc proc) :
  yaga::PostProcessor(order, hostMemory, proc),
  pool_(pool)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
PostProcessor::~PostProcessor()
{
  pool_->remove(this);
}

} // !namespace vk
} // !namespace yaga
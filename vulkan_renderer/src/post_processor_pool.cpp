#include "precompiled.h"
#include "vulkan_renderer/post_processor_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
PostProcessorPool::PostProcessorPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
PostProcessorPool::~PostProcessorPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void PostProcessorPool::clear()
{
  if (!processors_.empty()) {
    THROW("Not all post processors were returned to the pool");
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void PostProcessorPool::remove(PostProcessor* processor)
{
  processors_.erase(processor->order());
}

// -----------------------------------------------------------------------------------------------------------------------------
PostProcessorPtr PostProcessorPool::create(int order, bool hostMemory, PostProcessor::Proc proc)
{
  if (processors_.find(order) != processors_.end()) {
    THROW("Post processor with order %1% already exists", order);
  }
  auto processor = std::make_unique<PostProcessor>(this, order, hostMemory, proc);
  processors_[order] = processor.get();
  return std::move(processor);
}

} // !namespace vk
} // !namespace yaga
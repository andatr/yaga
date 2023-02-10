#ifndef YAGA_VULKAN_RENDERER_POST_PROCESSOR
#define YAGA_VULKAN_RENDERER_POST_PROCESSOR

#include <memory>

#include "vulkan_renderer/vulkan.h"
#include "engine/post_processor.h"
#include "utility/signal.h"

namespace yaga {
namespace vk {

class PostProcessorPool;

// -----------------------------------------------------------------------------------------------------------------------------
class PostProcessor : public yaga::PostProcessor
{
public:
  explicit PostProcessor(PostProcessorPool* pool, int order, bool hostMemory, Proc proc);
  virtual ~PostProcessor();
  const int order() const { return order_; }

private:
  PostProcessorPool* pool_;
};

typedef std::unique_ptr<PostProcessor> PostProcessorPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_POST_PROCESSOR

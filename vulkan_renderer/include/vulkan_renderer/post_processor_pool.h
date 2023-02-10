#ifndef YAGA_VULKAN_RENDERER_POST_PROCESSOR_POOL
#define YAGA_VULKAN_RENDERER_POST_PROCESSOR_POOL

#include <memory>
#include <map>

#include "vulkan_renderer/post_processor.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class PostProcessorPool
{
public:
  explicit PostProcessorPool();
  ~PostProcessorPool();
  PostProcessorPtr create(int order, bool hostMemory, PostProcessor::Proc proc);
  void remove(PostProcessor* processor);
  void clear();

private:
  std::map<int, PostProcessor*> processors_;
};

typedef std::unique_ptr<PostProcessorPool> PostProcessorPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_POST_PROCESSOR_POOL

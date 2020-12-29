#ifndef YAGA_VULKAN_RENDERER_SRC_RENDER_STAGE
#define YAGA_VULKAN_RENDERER_SRC_RENDER_STAGE

#include <memory>

#include "context.h"
#include "vulkan.h"

namespace yaga {
namespace vk {

class RenderStage
{
public:
  virtual ~RenderStage() {}
  virtual VkSemaphore render(Context* context, uint32_t frame, VkSemaphore waitFor) = 0;
};

typedef std::unique_ptr<RenderStage> RenderStagePtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDER_STAGE

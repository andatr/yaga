#ifndef YAGA_VULKAN_RENDERER_RENDERER
#define YAGA_VULKAN_RENDERER_RENDERER

#include <memory>
#include <vector>

#include "context.h"
#include "render_stage.h"
#include "swapchain.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Renderer
{
public:
  Renderer(Swapchain* swapchain, std::vector<RenderStagePtr>& stages);
  ~Renderer();
  bool render(Context* context);

private:
  void createSemaphore();
  
private:
  AutoDestructor<VkSemaphore> imageSync_;
  Swapchain* swapchain_;
  std::vector<RenderStagePtr> stages_;
};

typedef std::unique_ptr<Renderer> RendererPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_RENDERER

#ifndef YAGA_VULKAN_RENDERER_RENDERER
#define YAGA_VULKAN_RENDERER_RENDERER

#include <memory>
#include <vector>

#include "vulkan_renderer/render_pass.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Renderer
{
public:
  Renderer(Swapchain* swapchain, std::initializer_list<RenderPassPtr> passes = {});
  ~Renderer();
  std::vector<RenderPassPtr>& passes() { return passes_; }
  bool render();

private:
  AutoDestructor<VkSemaphore> imageSync_;
  Swapchain* swapchain_;
  std::vector<RenderPassPtr> passes_;
};

typedef std::unique_ptr<Renderer> RendererPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_RENDERER

#include "precompiled.h"
#include "vulkan_renderer/renderer.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(Swapchain* swapchain, std::initializer_list<RenderPassPtr> passes) :
  swapchain_(swapchain),
  passes_(passes)
{
  imageSync_ = createSemaphore(**swapchain_->device());
}

// -----------------------------------------------------------------------------------------------------------------------------
Renderer::~Renderer()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Renderer::render()
{
  VkSemaphore sync = *imageSync_;
  auto image = swapchain_->acquireImage(sync);
  if (image == Swapchain::BAD_IMAGE) return false;
  for (auto& pass : passes_) {
    sync = pass->render(image, sync);
  }
  return swapchain_->presentImage(sync, image);
}

} // !namespace vk
} // !namespace yaga
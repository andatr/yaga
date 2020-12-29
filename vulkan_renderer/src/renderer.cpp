#include "precompiled.h"
#include "renderer.h"
#include "render_stage_3d.h"
#include "render_stage_gui.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(Swapchain* swapchain, std::vector<RenderStagePtr>& stages) :
  swapchain_(swapchain),
  stages_(std::move(stages))
{
  createSemaphore();
}

// -----------------------------------------------------------------------------------------------------------------------------
Renderer::~Renderer()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Renderer::createSemaphore()
{
  auto deleteSemaphore = [device = **swapchain_->device()](auto semaphore) {
    vkDestroySemaphore(device, semaphore, nullptr);
    LOG(trace) << "Vulkan Semaphore destroyed";
  };
  VkSemaphore semaphore;
  VkSemaphoreCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VULKAN_GUARD(vkCreateSemaphore(**swapchain_->device(), &info, nullptr, &semaphore), "Could not create Vulkan Semaphore");
  imageSync_.set(semaphore, deleteSemaphore);
  LOG(trace) << "Vulkan Semaphore created";
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Renderer::render(Context* context)
{
  auto image = swapchain_->acquireImage(*imageSync_);
  if (image == Swapchain::BAD_IMAGE) {
    return false;
  }
  VkSemaphore sync = *imageSync_;
  for (auto& stage : stages_) {
    sync = stage->render(context, image, sync);
  }
  return swapchain_->presentImage(sync, image);
}

} // !namespace vk
} // !namespace yaga
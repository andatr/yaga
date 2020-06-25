#include "precompiled.h"
#include "presenter.h"
#include "assets/vertex.h"

namespace yaga
{
namespace vk
{
namespace
{

constexpr size_t MAX_FRAMES = 2;

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
Presenter::Presenter(Device* device, Swapchain* swapchain) :
  device_(device), vkDevice_(**device), swapchain_(**swapchain), frameSync_(MAX_FRAMES), frame_(0)
{
  createSync();
}

// -------------------------------------------------------------------------------------------------------------------------
void Presenter::swapchain(Swapchain* swapchain)
{
  swapchain_ = **swapchain;
}

// -------------------------------------------------------------------------------------------------------------------------
void Presenter::createSync()
{
  VkSemaphoreCreateInfo semaphoreInfo {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  auto deleteSemaphore = [this](auto semaphore) {
    vkDestroySemaphore(vkDevice_, semaphore, nullptr);
    LOG(trace) << "Vulkan Semaphore destroyed";
  };
  auto deleteFence = [this](auto fence) {
    vkDestroyFence(vkDevice_, fence, nullptr);
    LOG(trace) << "Vulkan Fence destroyed";
  };

  auto createSemaphore = [this, &semaphoreInfo](auto& semaphore) {
    VULKAN_GUARD(vkCreateSemaphore(vkDevice_, &semaphoreInfo, nullptr, &semaphore), "Could not create Vulkan Semaphore");
    LOG(trace) << "Vulkan Semaphore created";
  };

  VkFence fence;
  VkSemaphore semaphore;
  for (size_t i = 0; i < frameSync_.size(); ++i) {
    createSemaphore(semaphore);
    frameSync_[i].render.set(semaphore, deleteSemaphore);
    createSemaphore(semaphore);
    frameSync_[i].present.set(semaphore, deleteSemaphore);
    VULKAN_GUARD(vkCreateFence(vkDevice_, &fenceInfo, nullptr, &fence), "Could not create Vulkan Fence");
    frameSync_[i].swap.set(fence, deleteFence);
    LOG(trace) << "Vulkan Fence created";
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Presenter::waitPrevFrame()
{
  const auto& sync = frameSync_[frame_];
  vkWaitForFences(vkDevice_, 1, &*sync.swap, VK_TRUE, UINT64_MAX);
}

// -------------------------------------------------------------------------------------------------------------------------
bool Presenter::acquireImage(uint32_t* image)
{
  const auto& sync = frameSync_[frame_];
  auto result = vkAcquireNextImageKHR(vkDevice_, swapchain_, INT64_MAX, *sync.render, VK_NULL_HANDLE, image);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    return false;
  }
  if (result != VK_SUCCESS) {
    THROW("Failed to acquire swapchain image");
  }
  return true;
}

// -------------------------------------------------------------------------------------------------------------------------
bool Presenter::present(VkCommandBuffer command, uint32_t image)
{
  const auto& sync = frameSync_[frame_];
  frame_ = ++frame_ % MAX_FRAMES;

  VkSubmitInfo submitInfo {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &*sync.render;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &*sync.present;
  vkResetFences(vkDevice_, 1, &*sync.swap);
  VULKAN_GUARD(vkQueueSubmit(device_->graphicsQueue(), 1, &submitInfo, *sync.swap), "Could not draw frame");

  VkPresentInfoKHR presentInfo {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &*sync.present;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain_;
  presentInfo.pImageIndices = &image;
  auto result = vkQueuePresentKHR(device_->presentQueue(), &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    return false;
  }
  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    THROW("Could not present image");
  }
  return true;
}

} // !namespace vk
} // !namespace yaga
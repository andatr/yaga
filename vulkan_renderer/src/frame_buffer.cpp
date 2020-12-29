#include "precompiled.h"
#include "frame_buffer.h"
#include "render_pass.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
FrameBuffer::FrameBuffer(RenderPass* renderPass, VkImageView* attachments, size_t attachmentCount) :
  renderPass_(renderPass)
{
  auto device = **renderPass->device();
  createFrameBuffer(renderPass, attachments, attachmentCount);
  createFence(device);
  createSemaphore(device);
  createCommand(renderPass->device());
}

// -----------------------------------------------------------------------------------------------------------------------------
FrameBuffer::~FrameBuffer()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameBuffer::update(VkImageView* attachments, size_t attachmentCount)
{
  createFrameBuffer(renderPass_, attachments, attachmentCount);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameBuffer::createFrameBuffer(RenderPass* renderPass, VkImageView* attachments, size_t attachmentCount)
{
  auto device = **renderPass->device();
  auto destroyFrameBuffer = [device](auto frameBuffer) {
    vkDestroyFramebuffer(device, frameBuffer, nullptr);
    LOG(trace) << "Framebuffer destroyed";
  };
  VkFramebuffer frameBuffer;
  VkFramebufferCreateInfo info{};
  info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.renderPass      = **renderPass;
  info.pAttachments    = attachments;
  info.attachmentCount = static_cast<uint32_t>(attachmentCount);
  info.width           = renderPass->renderArea().extent.width;
  info.height          = renderPass->renderArea().extent.height;
  info.layers          = 1;
  VULKAN_GUARD(vkCreateFramebuffer(device, &info, nullptr, &frameBuffer), "Could not create Framebuffer");
  frameBuffer_.set(frameBuffer, destroyFrameBuffer);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameBuffer::createFence(VkDevice device)
{
  auto deleteFence = [device](auto fence) {
    vkDestroyFence(device, fence, nullptr);
    LOG(trace) << "Vulkan Fence destroyed";
  };
  VkFence fence;
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  VULKAN_GUARD(vkCreateFence(device, &fenceInfo, nullptr, &fence), "Could not create Vulkan Fence");
  syncSubmit_.set(fence, deleteFence);
  LOG(trace) << "Vulkan Fence created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameBuffer::createSemaphore(VkDevice device)
{
  auto deleteSemaphore = [device](auto semaphore) {
    vkDestroySemaphore(device, semaphore, nullptr);
    LOG(trace) << "Vulkan Semaphore destroyed";
  };
  VkSemaphore semaphore;
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VULKAN_GUARD(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore), "Could not create Vulkan Semaphore");
  syncRender_.set(semaphore, deleteSemaphore);
  LOG(trace) << "Vulkan Semaphore created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void FrameBuffer::createCommand(Device* device)
{
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = device->commandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VULKAN_GUARD(vkAllocateCommandBuffers(**device, &allocInfo, &command_), "Could not allocate Camera Command Buffers");
}

} // !namespace vk
} // !namespace yaga
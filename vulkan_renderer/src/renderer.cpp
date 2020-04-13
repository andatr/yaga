#include "precompiled.h"
#include "renderer.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "engine/vertex.h"

namespace yaga
{
namespace vk
{

// -------------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(Device* device, Swapchain* swapchain) :
  device_(device), vkDevice_(**device), swapchain_(swapchain),
  frames_(static_cast<uint32_t>(swapchain_->frameBuffers().size()))
{
  allocateCommandBuffer();
}

// -------------------------------------------------------------------------------------------------------------------------
void Renderer::swapchain(Swapchain* swapchain)
{
  swapchain_ = swapchain;
}

// -------------------------------------------------------------------------------------------------------------------------
void Renderer::allocateCommandBuffer()
{
  VkCommandBufferAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = device_->commandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = frames_;
  commandBuffers_.resize(frames_);
  VULKAN_GUARD(vkAllocateCommandBuffers(vkDevice_, &allocInfo, commandBuffers_.data()), "Could not allocate command buffers");
}

// -------------------------------------------------------------------------------------------------------------------------
VkCommandBuffer Renderer::render(Scene* scene, uint32_t frame)
{
  std::array<VkClearValue, 2> clearValues {};
  clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
  clearValues[1].depthStencil = { 1.0f, 0 };

  VkRenderPassBeginInfo renderPassInfo {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = swapchain_->renderPass();
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = swapchain_->resolution();
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();
  renderPassInfo.framebuffer = swapchain_->frameBuffers()[frame];

  auto command = commandBuffers_[frame];
  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  VULKAN_GUARD(vkBeginCommandBuffer(command, &beginInfo), "Failed to begin recording command buffer");

  // TODO: sort by material / buffers
  vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  renderModel(scene->root(), command, frame);
  vkCmdEndRenderPass(command);

  VULKAN_GUARD(vkEndCommandBuffer(command), "Failed to record command buffer");
  return command;
}

// -------------------------------------------------------------------------------------------------------------------------
void Renderer::renderModel(Object* object, VkCommandBuffer command, uint32_t frame) const
{
  auto vkModel = dynamic_cast<Model*>(object);
  if (!vkModel) return;
  auto mesh = dynamic_cast<Mesh*>(vkModel->mesh());
  auto material = dynamic_cast<Material*>(vkModel->material());
  if (!mesh || !material) return;

  VkBuffer vertexBuffers[] = { mesh->vertexBuffer() };
  VkDeviceSize offsets[] = { 0 };
  vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline());
  vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipelineLayout(), 0, 2,
    &material->descriptorSets()[frame * 2], 0, nullptr);
  vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(command, mesh->indexBuffer(), 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(command, mesh->indexCount(), 1, 0, 0, 0);
}

} // !namespace vk
} // !namespace yaga
#include "precompiled.h"
#include "renderer.h"
#include "material.h"
#include "mesh.h"
#include "uniform.h"
#include "assets/vertex.h"

namespace yaga {
namespace vk {

// -------------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(Swapchain* swapchain, RenderingContext* context) :
  swapchain_(swapchain), context_(context), frames_(static_cast<uint32_t>(swapchain->frameBuffers().size()))
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Renderer::render(uint32_t frame) const
{
  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = { 0.0f, 1.0f, 0.0f, 1.0f };
  clearValues[1].depthStencil = { 1.0f, 0 };

  VkRenderPassBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  info.renderPass = swapchain_->renderPass();
  info.renderArea.offset = { 0, 0 };
  info.renderArea.extent = swapchain_->resolution();
  info.clearValueCount = static_cast<uint32_t>(clearValues.size());
  info.pClearValues = clearValues.data();
  info.framebuffer = swapchain_->frameBuffers()[frame];

  for (const auto& camera : context_->cameras()) {
    auto& command = camera->frame(frame).command;
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VULKAN_GUARD(vkBeginCommandBuffer(command, &beginInfo), "Failed to begin recording Command Buffer");
    // TODO: add culling and sorting by material / buffers
    vkCmdBeginRenderPass(command, &info, VK_SUBPASS_CONTENTS_INLINE);
    for (const auto& object : context_->renderers3D()) {
      if (object->canRender()) {
        renderObject(camera, object, command, frame);
      }
    }
    vkCmdEndRenderPass(command);
    VULKAN_GUARD(vkEndCommandBuffer(command), "Failed to record Command Buffer");
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Renderer::renderObject(Camera* camera, Renderer3D* object, VkCommandBuffer command, uint32_t frame)
{
  const auto& mesh = object->mesh();
  const auto& material = object->material();
  VkBuffer vertexBuffers[] = { mesh->vertexBuffer() };
  VkDeviceSize offsets[] = { 0 };
  std::array<VkDescriptorSet, 2> descriptors{ camera->frame(frame).descriptor, material->descriptorSets()[frame] };
  const auto& pconst = object->pushConstant();
  vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline());
  vkCmdBindDescriptorSets(
    command, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipelineLayout(), 0, 2, descriptors.data(), 0, nullptr);
  vkCmdPushConstants(command, material->pipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantVertex), &pconst);
  vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(command, mesh->indexBuffer(), 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(command, mesh->indexCount(), 1, 0, 0, 0);
}

} // !namespace vk
} // !namespace yaga

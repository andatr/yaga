#include "precompiled.h"
#include "model.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Model::Model(Device* device, VideoBuffer* videoBuffer, Mesh* mesh, Material* material) :
  mesh_(mesh), material_(material)
{
  CreateCommandBuffer(device, videoBuffer);
}

// -------------------------------------------------------------------------------------------------------------------------
void Model::CreateCommandBuffer(Device* device, VideoBuffer* videoBuffer)
{
  commandBuffers_.resize(material_->FrameBuffers().size());
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = device->CommandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());
  if (vkAllocateCommandBuffers(device->Logical(), &allocInfo, commandBuffers_.data()) != VK_SUCCESS) {
    THROW("Could not allocate command buffers");
  }
  for (size_t i = 0; i < commandBuffers_.size(); i++)
  {
    const auto& command = commandBuffers_[i];

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(command, &beginInfo) != VK_SUCCESS) {
      THROW("Failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = material_->RenderPass();
    renderPassInfo.framebuffer = material_->FrameBuffers()[i];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = videoBuffer->Size();
    VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, material_->Pipeline());
    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, videoBuffer->PipelineLayout(), 0, 1,
      &videoBuffer->DescriptorSets()[i], 0, nullptr);
    VkBuffer vertexBuffers[] = { mesh_->VertexBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(command, mesh_->IndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command, static_cast<uint32_t>(mesh_->Indices().size()), 1, 0, 0, 0);
    vkCmdEndRenderPass(command);

    if (vkEndCommandBuffer(command) != VK_SUCCESS) {
      THROW("Failed to record command buffer");
    }
  }
}

} // !namespace yaga
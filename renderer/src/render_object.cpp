#include "precompiled.h"
#include "render_object.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
RenderObject::RenderObject(Device* device, Swapchain* swapchain, Mesh* mesh, Material* material, const std::vector<ImageView*>& textures) :
  device_(device), vkDevice_(**device), swapchain_(swapchain), mesh_(mesh), material_(material), textures_(textures)
{
  createDescriptorSets();
  createCommandBuffer();
}

// -------------------------------------------------------------------------------------------------------------------------
void RenderObject::createDescriptorSets()
{
  const auto frameCount = swapchain_->frames().size();

  std::vector<VkDescriptorSetLayout> layouts(frameCount, swapchain_->descriptorSetLayout());
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = swapchain_->descriptorPool();
  allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets_.resize(frameCount);
  if (vkAllocateDescriptorSets(vkDevice_, &allocInfo, descriptorSets_.data()) != VK_SUCCESS) {
    throw std::runtime_error("Could not allocate descriptor sets");
  }

  for (size_t i = 0; i < frameCount; i++) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = **swapchain_->uniformBuffers()[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformObject);

    std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets_[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    if (textures_.size() > 0)
    {
      VkDescriptorImageInfo imageInfo = {};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = **textures_[0];
      imageInfo.sampler = swapchain_->textureSampler();

      descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[1].dstSet = descriptorSets_[i];
      descriptorWrites[1].dstBinding = 1;
      descriptorWrites[1].dstArrayElement = 0;
      descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrites[1].descriptorCount = 1;
      descriptorWrites[1].pImageInfo = &imageInfo;
    }

    vkUpdateDescriptorSets(vkDevice_, static_cast<uint32_t>(textures_.size() > 0 ? descriptorWrites.size() : 1), descriptorWrites.data(), 0, nullptr);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void RenderObject::createCommandBuffer()
{
  commandBuffers_.resize(material_->frameBuffers().size());
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = device_->commandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());
  if (vkAllocateCommandBuffers(vkDevice_, &allocInfo, commandBuffers_.data()) != VK_SUCCESS) {
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

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = material_->renderPass();
    renderPassInfo.framebuffer = material_->frameBuffers()[i];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain_->size();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, material_->pipeline());
    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, swapchain_->pipelineLayout(), 0, 1,
      &descriptorSets_[i], 0, nullptr);
    VkBuffer vertexBuffers[] = { mesh_->vertexBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(command, mesh_->indexBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(command, static_cast<uint32_t>(mesh_->indices().size()), 1, 0, 0, 0);
    vkCmdEndRenderPass(command);

    if (vkEndCommandBuffer(command) != VK_SUCCESS) {
      THROW("Failed to record command buffer");
    }
  }
}

} // !namespace yaga
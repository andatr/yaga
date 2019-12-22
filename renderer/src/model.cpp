#include "precompiled.h"
#include "model.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Model::Model(Device* device, VideoBuffer* videoBuffer, Mesh* mesh, Material* material, const std::vector<ImageView*>& textures) :
  device_(device->Logical()), videoBuffer_(videoBuffer), mesh_(mesh), material_(material), textures_(textures)
{
  CreateDescriptorSets(device->Logical(), videoBuffer);
  CreateCommandBuffer(device, videoBuffer);
}

// -------------------------------------------------------------------------------------------------------------------------
void Model::CreateDescriptorSets(VkDevice device, VideoBuffer* videoBuffer)
{
  const auto frameCount = videoBuffer->Frames().size();

  std::vector<VkDescriptorSetLayout> layouts(frameCount, videoBuffer->DescriptorSetLayout());
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = videoBuffer->DescriptorPool();
  allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets_.resize(frameCount);
  if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets_.data()) != VK_SUCCESS) {
    throw std::runtime_error("Could not allocate descriptor sets");
  }

  for (size_t i = 0; i < frameCount; i++) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = **videoBuffer->UniformBuffers()[i];
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
      imageInfo.sampler = videoBuffer->TextureSampler();

      descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[1].dstSet = descriptorSets_[i];
      descriptorWrites[1].dstBinding = 1;
      descriptorWrites[1].dstArrayElement = 0;
      descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrites[1].descriptorCount = 1;
      descriptorWrites[1].pImageInfo = &imageInfo;
    }

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(textures_.size() > 0 ? descriptorWrites.size() : 1), descriptorWrites.data(), 0, nullptr);
  }
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

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = material_->RenderPass();
    renderPassInfo.framebuffer = material_->FrameBuffers()[i];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = videoBuffer->Size();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, material_->Pipeline());
    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, videoBuffer->PipelineLayout(), 0, 1,
      &descriptorSets_[i], 0, nullptr);
    VkBuffer vertexBuffers[] = { mesh_->VertexBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(command, mesh_->IndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(command, static_cast<uint32_t>(mesh_->Indices().size()), 1, 0, 0, 0);
    vkCmdEndRenderPass(command);

    if (vkEndCommandBuffer(command) != VK_SUCCESS) {
      THROW("Failed to record command buffer");
    }
  }
}

} // !namespace yaga
#include "precompiled.h"
#include "mesh.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(Device* device, Allocator* allocator, asset::Mesh* asset) :
  device_(device), asset_(asset)
{
  CreateVertexBuffer(device->Logical(), allocator);
  CreateIndexBuffer(device->Logical(), allocator);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::CreateVertexBuffer(VkDevice device, Allocator* allocator)
{
  auto size = static_cast<VkDeviceSize>(sizeof(Vertex) * asset_->Vertices().size());
  vertexBuffer_ = std::make_unique<DeviceBuffer>(device, allocator, size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  auto stageBuffer = std::make_unique<DeviceBuffer>(device, allocator, size,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stageBuffer->Update(asset_->Vertices().data(), size);
  CopyBuffer(vertexBuffer_->Buffer(), stageBuffer->Buffer(), size);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::CreateIndexBuffer(VkDevice device, Allocator* allocator)
{
  auto size = sizeof(asset_->Indices()[0]) * asset_->Indices().size();
  indexBuffer_ = std::make_unique<DeviceBuffer>(device, allocator, size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  auto stageBuffer = std::make_unique<DeviceBuffer>(device, allocator, size,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stageBuffer->Update(asset_->Indices().data(), size);
  CopyBuffer(indexBuffer_->Buffer(), stageBuffer->Buffer(), size);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::CopyBuffer(VkBuffer destination, VkBuffer source, VkDeviceSize size) const
{
  auto device = device_->Logical();

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = device_->CommandPool();
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);
  VkBufferCopy copyRegion = {};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, source, destination, 1, &copyRegion);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(device_->GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(device_->GraphicsQueue());

  vkFreeCommandBuffers(device, device_->CommandPool(), 1, &commandBuffer);
}

} // !namespace yaga
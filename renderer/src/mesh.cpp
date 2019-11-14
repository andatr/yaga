#include "precompiled.h"
#include "mesh.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(Device* device, Allocator* allocator, asset::Mesh* asset) :
  device_(device), allocator_(allocator), asset_(asset)
{
  Rebuild();
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::Rebuild()
{
  CreateVertexBuffer();
  CreateIndexBuffer();
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::CreateVertexBuffer()
{
  auto size = static_cast<VkDeviceSize>(sizeof(Vertex) * asset_->Vertices().size());

  auto stageBuffer = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  auto stageMemory = allocator_->Allocate(*stageBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  vkBindBufferMemory(device_->Logical(), *stageBuffer, *stageMemory, 0);

  void* data;
  const auto device = device_->Logical();
  vkMapMemory(device, *stageMemory, 0, size, 0, &data);
  memcpy(data, asset_->Vertices().data(), static_cast<size_t>(size));
  vkUnmapMemory(device, *stageMemory);

  vertexBuffer_ = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  vertexMemory_ = allocator_->Allocate(*vertexBuffer_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkBindBufferMemory(device_->Logical(), *vertexBuffer_, *vertexMemory_, 0);

  CopyBuffer(*vertexBuffer_, *stageBuffer, size);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::CreateIndexBuffer()
{
  auto size = sizeof(asset_->Indices()[0]) * asset_->Indices().size();

  auto stageBuffer = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  auto stageMemory = allocator_->Allocate(*stageBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  vkBindBufferMemory(device_->Logical(), *stageBuffer, *stageMemory, 0);

  void* data;
  const auto device = device_->Logical();
  vkMapMemory(device, *stageMemory, 0, size, 0, &data);
  memcpy(data, asset_->Indices().data(), static_cast<size_t>(size));
  vkUnmapMemory(device, *stageMemory);

  indexBuffer_ = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  indexMemory_ = allocator_->Allocate(*indexBuffer_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkBindBufferMemory(device_->Logical(), *indexBuffer_, *indexMemory_, 0);

  CopyBuffer(*indexBuffer_, *stageBuffer, size);
}

// -------------------------------------------------------------------------------------------------------------------------
AutoDestroyer<VkBuffer> Mesh::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const
{
  const auto device = device_->Logical();
  VkBufferCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = size;
  info.usage = usage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  auto deleteBuffer = [device](auto buffer) {
    vkDestroyBuffer(device, buffer, nullptr);
    LOG(trace) << "Vertex buffer destroyed";
  };
  VkBuffer buffer;
  if (vkCreateBuffer(device, &info, nullptr, &buffer) != VK_SUCCESS) {
    THROW("Could not create vertex buffer");
  }
  AutoDestroyer<VkBuffer> smartBuffer;
  smartBuffer.Assign(buffer, deleteBuffer);
  return smartBuffer;
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
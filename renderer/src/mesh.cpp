#include "precompiled.h"
#include "mesh.h"
#include "device.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(Device* device) : device_(device)
{
  vertices_ = {
    {{  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
    {{  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
    {{ -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }}
  };
  Rebuild();
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::Rebuild()
{
  CreateVertexBuffer();
  CreateStageBuffer();
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::CreateVertexBuffer()
{
  auto size = static_cast<VkDeviceSize>(sizeof(Vertex) * vertices_.size());
  vertexBuffer_ = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  vertexMemory_ = AllocateMemory(*vertexBuffer_, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::CreateStageBuffer()
{
  auto size = static_cast<VkDeviceSize>(sizeof(Vertex) * vertices_.size());
  stageBuffer_ = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  stageMemory_ = AllocateMemory(*stageBuffer_, size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  const auto device = device_->Logical();
  void* data;
  vkBindBufferMemory(device, *stageBuffer_, *stageMemory_, 0);
  vkMapMemory(device, *stageMemory_, 0, size, 0, &data);
  memcpy(data, vertices_.data(), static_cast<size_t>(size));
  vkUnmapMemory(device, *stageMemory_);
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
    LOG(trace) << "Vertex buffer deleted";
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
AutoDestroyer<VkDeviceMemory> Mesh::AllocateMemory(VkBuffer buffer, VkDeviceSize size,VkMemoryPropertyFlags properties) const
{
  const auto device = device_->Logical();
  VkMemoryRequirements requirements;
  vkGetBufferMemoryRequirements(device, buffer, &requirements);

  VkMemoryAllocateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  info.allocationSize = requirements.size;
  info.memoryTypeIndex = device_->GetMemoryType(requirements.memoryTypeBits, properties);

  auto freeMemory = [device](auto memory) {
    vkFreeMemory(device, memory, nullptr);
    LOG(trace) << "Vertex buffer memory released";
  };
  VkDeviceMemory memory;
  if (vkAllocateMemory(device, &info, nullptr, &memory) != VK_SUCCESS) {
    THROW("Could not allocate memory for vertex buffer");
  }
  AutoDestroyer<VkDeviceMemory> smartMemory;
  smartMemory.Assign(memory, freeMemory);
  return smartMemory;
}

// -------------------------------------------------------------------------------------------------------------------------
//void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
//{
  /*VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  VkBufferCopy copyRegion = {};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue);

  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);*/
//}

} // !namespace yaga
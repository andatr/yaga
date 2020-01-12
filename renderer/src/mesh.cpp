#include "precompiled.h"
#include "mesh.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(Device* device, Allocator* allocator, asset::Mesh* asset) :
  device_(device), vkDevice_(**device), allocator_(allocator), asset_(asset)
{
  createVertexBuffer();
  createIndexBuffer();
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::createVertexBuffer()
{
  auto size = static_cast<VkDeviceSize>(sizeof(Vertex) * asset_->vertices().size());
  vertexBuffer_ = std::make_unique<DeviceBuffer>(device_, allocator_, size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  auto stageBuffer = std::make_unique<DeviceBuffer>(device_, allocator_, size,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stageBuffer->update(asset_->vertices().data(), size);
  copyBuffer(**vertexBuffer_, **stageBuffer, size);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::createIndexBuffer()
{
  auto size = sizeof(asset_->indices()[0]) * asset_->indices().size();
  indexBuffer_ = std::make_unique<DeviceBuffer>(device_, allocator_, size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  auto stageBuffer = std::make_unique<DeviceBuffer>(device_, allocator_, size,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stageBuffer->update(asset_->indices().data(), size);
  copyBuffer(**indexBuffer_, **stageBuffer, size);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::copyBuffer(VkBuffer destination, VkBuffer source, VkDeviceSize size) const
{
  device_->submitCommand([destination, source, size](auto cmd) {
    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(cmd, source, destination, 1, &copyRegion);
  });
}

} // !namespace yaga
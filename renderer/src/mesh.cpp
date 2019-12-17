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
  CopyBuffer(**vertexBuffer_, **stageBuffer, size);
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
  CopyBuffer(**indexBuffer_, **stageBuffer, size);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::CopyBuffer(VkBuffer destination, VkBuffer source, VkDeviceSize size) const
{
  device_->SubmitCommand([destination, source, size](auto cmd) {
    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(cmd, source, destination, 1, &copyRegion);
  });
}

} // !namespace yaga
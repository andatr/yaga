#include "precompiled.h"
#include "mesh_pool.h"

namespace yaga
{
namespace vk
{
namespace
{

// -------------------------------------------------------------------------------------------------------------------------
void copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandBuffer command)
{
  VkBufferCopy copyRegion {};
  copyRegion.size = size;
  vkCmdCopyBuffer(command, source, destination, 1, &copyRegion);
}

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
MeshPool::MeshPool(Device* device, VmaAllocator allocator, uint32_t maxVertexCount, uint32_t maxIndexCount) :
  device_(device), vkDevice_(**device), allocator_(allocator), maxVertexCount_(maxVertexCount), maxIndexCount_(maxIndexCount)
{
  createStageBuffers(maxVertexCount, maxIndexCount);
}

// -------------------------------------------------------------------------------------------------------------------------
Mesh* MeshPool::createMesh(asset::Mesh* asset)
{
  auto it = meshes_.find(asset);
  if (it != meshes_.end()) return it->second.get();

  auto indexCount = static_cast<uint32_t>(asset->indices().size());
  auto vertexSize = static_cast<VkDeviceSize>(sizeof(asset->vertices()[0]) * asset->vertices().size());
  auto indexSize = static_cast<VkDeviceSize>(sizeof(asset->indices()[0])  * indexCount);
  if (indexCount > maxIndexCount_) THROW("mesh exceed max index count");
  if (asset->vertices().size() > maxVertexCount_) THROW("mesh exceed max vertex count");

  void* mappedData;
  vmaMapMemory(allocator_, stageVertexBuffer_->allocation(), &mappedData);
  memcpy(mappedData, asset->vertices().data(), vertexSize);
  vmaUnmapMemory(allocator_, stageVertexBuffer_->allocation());

  vmaMapMemory(allocator_, stageIndexBuffer_->allocation(), &mappedData);
  memcpy(mappedData, asset->indices().data(), indexSize);
  vmaUnmapMemory(allocator_, stageIndexBuffer_->allocation());

  auto mesh = std::make_unique<Mesh>(allocator_, vertexSize, indexSize, indexCount);
  device_->submitCommand([stageVertex = **stageVertexBuffer_, stageIndex = **stageIndexBuffer_,
    vertexSize, indexSize, mesh = mesh.get()](auto command)
  {
    copyBuffer(stageVertex, mesh->vertexBuffer(), vertexSize, command);
    copyBuffer(stageIndex,  mesh->indexBuffer(),  indexSize,  command);
  });

  auto meshPtr = mesh.get();
  meshes_[asset] = std::move(mesh);
  return meshPtr;
}

// -------------------------------------------------------------------------------------------------------------------------
void MeshPool::createStageBuffers(uint32_t maxVertexCount, uint32_t maxIndexCount)
{
  VkBufferCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = sizeof(Vertex) * maxVertexCount;
  info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo {};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
  allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

  stageVertexBuffer_ = std::make_unique<Buffer>(allocator_, info, allocInfo);
  info.size = sizeof(uint32_t) * maxIndexCount;
  stageIndexBuffer_ = std::make_unique<Buffer>(allocator_, info, allocInfo);
}

} // !namespace vk
} // !namespace yaga
#include "precompiled.h"
#include "mesh_pool.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
void copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandBuffer command)
{
  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(command, source, destination, 1, &copyRegion);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
MeshPool::MeshPool(Device* device, VmaAllocator allocator, const assets::Application* limits) :
  counter_(0),
  device_(device),
  allocator_(allocator),
  maxVertexCount_(limits->maxVertexCount()),
  maxIndexCount_(limits->maxIndexCount())
{
  createStageBuffers(maxVertexCount_, maxIndexCount_);
}

// -----------------------------------------------------------------------------------------------------------------------------
MeshPool::~MeshPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshPool::onRemove(Mesh*)
{
  if (counter_ > 0) {
    --counter_;
  }
  else {
    THROW("MeshPool::onRemove something went wrong");
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshPool::clear() 
{
  if (counter_ != 0) {
    THROW("Not all meshes were returned to the pool");
  }
  meshes_.clear();
}

// -----------------------------------------------------------------------------------------------------------------------------
MeshPtr MeshPool::get(Object* object, assets::Mesh* asset)
{
  auto indexCount   = static_cast<uint32_t>(asset->indices().size());
  auto vertexCount  = static_cast<uint32_t>(asset->vertices().size());
  if (indexCount  > maxIndexCount_)  THROW("Mesh exceed max index count");
  if (vertexCount > maxVertexCount_) THROW("Mesh exceed max vertex count");
  auto verticesSize = static_cast<VkDeviceSize>(sizeof(asset->vertices()[0]) * vertexCount);
  auto indicesSize  = static_cast<VkDeviceSize>(sizeof(asset->indices()[0])  * indexCount);
  ++counter_;

  auto it = meshes_.find(asset);
  if (it != meshes_.end()) {
    return std::make_unique<Mesh>(this, object, asset, it->second.vertices.get(), it->second.indices.get(),
      vertexCount, indexCount);
  }
   
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size  = verticesSize;
  info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  allocInfo.flags = 0;

  auto vertexBuffer = std::make_unique<Buffer>(allocator_, info, allocInfo);
  info.size = indicesSize;
  info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  auto indexBuffer = std::make_unique<Buffer>(allocator_, info, allocInfo);
  auto mesh = std::make_unique<Mesh>(this, object, asset, vertexBuffer.get(), indexBuffer.get(), vertexCount, indexCount);

  void* mappedData;
  vmaMapMemory(allocator_, stageVertexBuffer_->allocation(), &mappedData);
  memcpy(mappedData, asset->vertices().data(), verticesSize);
  vmaUnmapMemory(allocator_, stageVertexBuffer_->allocation());

  vmaMapMemory(allocator_, stageIndexBuffer_->allocation(), &mappedData);
  memcpy(mappedData, asset->indices().data(), indicesSize);
  vmaUnmapMemory(allocator_, stageIndexBuffer_->allocation());

  device_->submitCommand([
    stageVertex = **stageVertexBuffer_,
    stageIndex  = **stageIndexBuffer_,
    verticesSize,
    indicesSize,
    mesh = mesh.get()
  ](auto command) {
    copyBuffer(stageVertex, **mesh->vertexBuffer(), verticesSize, command);
    copyBuffer(stageIndex,  **mesh->indexBuffer(),  indicesSize,  command);
  });

  meshes_[asset] = {
    std::move(vertexBuffer),
    std::move(indexBuffer)
  };
  return std::move(mesh);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshPool::createStageBuffers(uint32_t maxVertexCount, uint32_t maxIndexCount)
{
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = sizeof(Vertex) * maxVertexCount;
  info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
  allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

  stageVertexBuffer_ = std::make_unique<Buffer>(allocator_, info, allocInfo);
  info.size = sizeof(Index) * maxIndexCount;
  stageIndexBuffer_ = std::make_unique<Buffer>(allocator_, info, allocInfo);
}

} // !namespace vk
} // !namespace yaga
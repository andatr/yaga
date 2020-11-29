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
MeshPool::MeshPool(Device* device, VmaAllocator allocator, uint32_t maxVertexCount, uint32_t maxIndexCount) :
  device_(device), vkDevice_(**device), allocator_(allocator), maxVertexCount_(maxVertexCount), maxIndexCount_(maxIndexCount)
{
  createStageBuffers(maxVertexCount, maxIndexCount);
}

// -----------------------------------------------------------------------------------------------------------------------------
MeshPool::~MeshPool()
{
  if (meshes_.empty()) {
    LOG(warning) << "Vulkan Mesh Pool memory leak";
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshPool::clear()
{
  if (!meshes_.empty()) {
    THROW("Can not clear Material Pool while its components are still in use");
  }
  meshCache_.clear();
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshPool::removeMesh(Mesh* mesh)
{
  vkDeviceWaitIdle(vkDevice_);
  meshes_.erase(mesh);
}

// -----------------------------------------------------------------------------------------------------------------------------
MeshPtr MeshPool::createMesh(Object* object, assets::Mesh* asset)
{
  auto it = meshCache_.find(asset);
  if (it != meshCache_.end()) {
    auto mesh =
      std::make_unique<Mesh>(object, asset, this, **it->second.vertexBuffer, **it->second.indexBuffer, it->second.indexCount);
    meshes_.insert(mesh.get());
    return mesh;
  }

  auto indexCount = static_cast<uint32_t>(asset->indices().size());
  auto verticesSize = static_cast<VkDeviceSize>(sizeof(asset->vertices()[0]) * asset->vertices().size());
  auto indicesSize = static_cast<VkDeviceSize>(sizeof(asset->indices()[0]) * indexCount);
  if (indexCount > maxIndexCount_) THROW("mesh exceed max index count");
  if (asset->vertices().size() > maxVertexCount_) THROW("mesh exceed max vertex count");

  void* mappedData;
  vmaMapMemory(allocator_, stageVertexBuffer_->allocation(), &mappedData);
  memcpy(mappedData, asset->vertices().data(), verticesSize);
  vmaUnmapMemory(allocator_, stageVertexBuffer_->allocation());

  vmaMapMemory(allocator_, stageIndexBuffer_->allocation(), &mappedData);
  memcpy(mappedData, asset->indices().data(), indicesSize);
  vmaUnmapMemory(allocator_, stageIndexBuffer_->allocation());

  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = verticesSize;
  info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  allocInfo.flags = 0;

  auto vertexBuffer = std::make_unique<Buffer>(allocator_, info, allocInfo);
  info.size = indicesSize;
  info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  auto indexBuffer = std::make_unique<Buffer>(allocator_, info, allocInfo);

  auto mesh = std::make_unique<Mesh>(object, asset, this, **vertexBuffer, **indexBuffer, indexCount);
  device_->submitCommand([
    stageVertex = **stageVertexBuffer_,
    stageIndex  = **stageIndexBuffer_,
    verticesSize,
    indicesSize,
    mesh = mesh.get()
  ](auto command) {
    copyBuffer(stageVertex, mesh->vertexBuffer(), verticesSize, command);
    copyBuffer(stageIndex, mesh->indexBuffer(), indicesSize, command);
  });

  MeshCache cache;
  cache.vertexBuffer = std::move(vertexBuffer);
  cache.indexBuffer = std::move(indexBuffer);
  cache.indexCount = indexCount;
  meshCache_[asset] = std::move(cache);

  meshes_.insert(mesh.get());
  return mesh;
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
  info.size = sizeof(uint32_t) * maxIndexCount;
  stageIndexBuffer_ = std::make_unique<Buffer>(allocator_, info, allocInfo);
}

} // !namespace vk
} // !namespace yaga
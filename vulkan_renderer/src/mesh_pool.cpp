#include "precompiled.h"
#include "vulkan_renderer/mesh_pool.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
std::pair<VkBufferCreateInfo, VmaAllocationCreateInfo> getBufferInfo(VkDeviceSize size, VkBufferUsageFlagBits usage)
{
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size  = size;
  info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  allocInfo.flags = 0;

  return std::make_pair(info, allocInfo);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
MeshPool::MeshPool(Device* device, VmaAllocator allocator, const Config::Resources& config) :
  counter_(0),
  device_(device),
  allocator_(allocator),
  maxVertexCount_(config.maxVertexCount()),
  maxIndexCount_(config.maxIndexCount())
{
  createStageBuffers(maxVertexCount_, maxIndexCount_);
}

// -----------------------------------------------------------------------------------------------------------------------------
MeshPool::~MeshPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshPool::remove(Mesh*)
{
  if (counter_ > 0) {
    --counter_;
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
MeshPtr MeshPool::get(assets::MeshPtr asset)
{
  auto it = meshes_.find(asset);
  if (it != meshes_.end()) {
    ++counter_;
    return std::make_unique<Mesh>(this, asset, it->second.vertices.get(), it->second.indices.get());
  }
  const auto indexCount   = static_cast<uint32_t>(asset->indices().size());
  const auto vertexCount  = static_cast<uint32_t>(asset->vertices().size());
  if (indexCount  > maxIndexCount_)  THROW("Mesh exceed max index count");
  if (vertexCount > maxVertexCount_) THROW("Mesh exceed max vertex count");
  const auto verticesSize = static_cast<VkDeviceSize>(sizeof(asset->vertices()[0]) * vertexCount);
  const auto indicesSize  = static_cast<VkDeviceSize>(sizeof(asset->indices()[0])  * indexCount);
  
  auto vertexBufferInfo = getBufferInfo(verticesSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  auto indexBufferInfo  = getBufferInfo(indicesSize,  VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  auto vertexBuffer     = std::make_unique<Buffer>(allocator_, vertexBufferInfo.first, vertexBufferInfo.second);
  auto indexBuffer      = std::make_unique<Buffer>(allocator_, indexBufferInfo.first,  indexBufferInfo.second);
  auto mesh             = std::make_unique<Mesh>(this, asset, vertexBuffer.get(), indexBuffer.get());
  
  meshes_[asset] = { std::move(vertexBuffer), std::move(indexBuffer) };
  update(mesh.get());
  ++counter_;
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

// -----------------------------------------------------------------------------------------------------------------------------
void MeshPool::update(Mesh* mesh)
{
  auto asset = mesh->asset();

  const auto indexCount   = static_cast<uint32_t>(asset->indices().size());
  const auto vertexCount  = static_cast<uint32_t>(asset->vertices().size());
  if (indexCount  > maxIndexCount_)  THROW("Mesh exceed max index count");
  if (vertexCount > maxVertexCount_) THROW("Mesh exceed max vertex count");
  const auto verticesSize = static_cast<VkDeviceSize>(sizeof(asset->vertices()[0]) * vertexCount);
  const auto indicesSize  = static_cast<VkDeviceSize>(sizeof(asset->indices()[0])  * indexCount);
  
  if (mesh->vertexBuffer()->memory().size < verticesSize) {
    auto info = getBufferInfo(verticesSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    mesh->vertexBuffer()->update(info.first, info.second);
  }
  if (mesh->indexBuffer()->memory().size < indicesSize) {
    auto info = getBufferInfo(indicesSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    mesh->indexBuffer()->update(info.first, info.second);
  }

  void* mappedData = nullptr;
  if (mesh->vertexDirty()) {
    vmaMapMemory(allocator_, stageVertexBuffer_->allocation(), &mappedData);
    memcpy(mappedData, asset->vertices().data(), verticesSize);
    vmaUnmapMemory(allocator_, stageVertexBuffer_->allocation());
  }
  if (mesh->indexDirty()) {
    vmaMapMemory(allocator_, stageIndexBuffer_->allocation(), &mappedData);
    memcpy(mappedData, asset->indices().data(), indicesSize);
    vmaUnmapMemory(allocator_, stageIndexBuffer_->allocation());
  }
  device_->submitCommand([
    stageVertex = **stageVertexBuffer_,
    stageIndex  = **stageIndexBuffer_,
    verticesSize,
    indicesSize,
    mesh
  ](auto command) {
    if (mesh->vertexDirty()) {
      copyBuffer(stageVertex, **mesh->vertexBuffer(), verticesSize, command);
    }
    if (mesh->indexDirty()) {
      copyBuffer(stageIndex,  **mesh->indexBuffer(),  indicesSize,  command);
    }
  });
}

} // !namespace vk
} // !namespace yaga
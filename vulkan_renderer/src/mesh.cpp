#include "precompiled.h"
#include "mesh.h"

namespace yaga
{
namespace vk
{

// -------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(VmaAllocator allocator, VkDeviceSize verticesSize, VkDeviceSize indicesSize, uint32_t indexCount) :
  indexCount_(indexCount)
{
  VkBufferCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = verticesSize;
  info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo {};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  allocInfo.flags = 0;

  vertexBuffer_ = std::make_unique<Buffer>(allocator, info, allocInfo);
  info.size = indicesSize;
  info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  indexBuffer_ = std::make_unique<Buffer>(allocator, info, allocInfo);
}

} // !namespace vk
} // !namespace yaga
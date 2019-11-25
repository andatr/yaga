#include "precompiled.h"
#include "device_buffer.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
DeviceBuffer::DeviceBuffer(VkDevice device, Allocator* allocator, VkDeviceSize size,
  VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryUsage) :
  device_(device)
{
  VkBufferCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = size;
  info.usage = bufferUsage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  auto deleteBuffer = [device](auto buffer) {
    vkDestroyBuffer(device, buffer, nullptr);
    LOG(trace) << "Device Buffer destroyed";
  };
  VkBuffer buffer;
  if (vkCreateBuffer(device_, &info, nullptr, &buffer) != VK_SUCCESS) {
    THROW("Could not create Device Buffer");
  }
  buffer_.Assign(buffer, deleteBuffer);

  memory_ = allocator->Allocate(*buffer_, memoryUsage);
  vkBindBufferMemory(device_, *buffer_, *memory_, 0);
}

// -------------------------------------------------------------------------------------------------------------------------
void DeviceBuffer::Update(const void* data, size_t size) const
{
  void* mapped;
  vkMapMemory(device_, *memory_, 0, size, 0, &mapped);
  memcpy(mapped, data, size);
  vkUnmapMemory(device_, *memory_);
}

} // !namespace yaga
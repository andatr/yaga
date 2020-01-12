#include "precompiled.h"
#include "device_buffer.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
DeviceBuffer::DeviceBuffer(Device* device, Allocator* allocator, VkDeviceSize size,
  VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryUsage) :
  device_(device), vkDevice_(**device), allocator_(allocator)
{
  VkBufferCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = size;
  info.usage = bufferUsage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  auto deleteBuffer = [device = vkDevice_](auto buffer) {
    vkDestroyBuffer(device, buffer, nullptr);
    LOG(trace) << "Device Buffer destroyed";
  };
  VkBuffer buffer;
  if (vkCreateBuffer(vkDevice_, &info, nullptr, &buffer) != VK_SUCCESS) {
    THROW("Could not create Device Buffer");
  }
  buffer_.set(buffer, deleteBuffer);

  memory_ = allocator->allocate(*buffer_, memoryUsage);
  vkBindBufferMemory(vkDevice_, *buffer_, *memory_, 0);
}

// -------------------------------------------------------------------------------------------------------------------------
void DeviceBuffer::update(const void* data, size_t size) const
{
  void* mapped;
  vkMapMemory(vkDevice_, *memory_, 0, size, 0, &mapped);
  memcpy(mapped, data, size);
  vkUnmapMemory(vkDevice_, *memory_);
}

} // !namespace yaga
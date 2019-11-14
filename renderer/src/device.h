#ifndef YAGA_RENDERER_SRC_DEVICE
#define YAGA_RENDERER_SRC_DEVICE

#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "utility/auto_destroyer.h"

namespace yaga
{

class Device : private boost::noncopyable
{
public:
  struct QueueFamilyIndices
  {
    uint32_t graphics;
    uint32_t surface;
    uint32_t compute;
    uint32_t transfer;
    //uint32_t sparceMemory;
    //uint32_t protectedMemory;
  };
public:
  explicit Device(VkInstance instance, VkSurfaceKHR surface);
  virtual ~Device();
  VkDevice Logical() const { return *logicalDevice_; }
  VkPhysicalDevice Physical() const { return physicalDevice_; }
  const QueueFamilyIndices& QueueFamilies() const { return queueFamilies_; }
  VkQueue GraphicsQueue() const { return queues_[0]; }
  VkQueue PresentQueue()  const { return queues_[1]; }
  VkQueue TransferQueue() const { return queues_[2]; }
  VkQueue ComputeQueue()  const { return queues_[3]; }
  uint32_t GetMemoryType(uint32_t filter, VkMemoryPropertyFlags props) const;
  VkCommandPool CommandPool() const { return *commandPool_; }
private:
  void CreateDevice();
  void CreateCommandPool();
private:
  VkPhysicalDevice physicalDevice_;
  VkPhysicalDeviceMemoryProperties memoryProperties_;
  AutoDestroyer<VkDevice> logicalDevice_;
  QueueFamilyIndices queueFamilies_;
  VkQueue queues_[4];
  AutoDestroyer<VkCommandPool> commandPool_;
};

typedef std::unique_ptr<Device> DevicePtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_DEVICE

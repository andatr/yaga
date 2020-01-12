#ifndef YAGA_RENDERER_SRC_DEVICE
#define YAGA_RENDERER_SRC_DEVICE

#include <functional>
#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "utility/auto_destructor.h"

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
  typedef std::function<void(VkCommandBuffer cmd)> CommandHandler;
public:
  explicit Device(VkInstance instance, VkSurfaceKHR surface);
  virtual ~Device();
  VkDevice operator*() const { return *logicalDevice_; }
  VkPhysicalDevice physical() const { return physicalDevice_; }
  const QueueFamilyIndices& queueFamilies() const { return queueFamilies_; }
  VkQueue graphicsQueue() const { return queues_[0]; }
  VkQueue presentQueue()  const { return queues_[1]; }
  VkQueue transferQueue() const { return queues_[2]; }
  VkQueue computeQueue()  const { return queues_[3]; }
  uint32_t getMemoryType(uint32_t filter, VkMemoryPropertyFlags props) const;
  VkCommandPool commandPool() const { return *commandPool_; }
  void submitCommand(CommandHandler handler) const;
private:
  void createDevice();
  void createCommandPool();
private:
  VkPhysicalDevice physicalDevice_;
  VkPhysicalDeviceMemoryProperties memoryProperties_;
  AutoDestructor<VkDevice> logicalDevice_;
  QueueFamilyIndices queueFamilies_;
  VkQueue queues_[4];
  AutoDestructor<VkCommandPool> commandPool_;
};

typedef std::unique_ptr<Device> DevicePtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_DEVICE

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
  struct DeviceQueues
  {
    std::vector<uint32_t> graphics;
    std::vector<uint32_t> compute;
    std::vector<uint32_t> transfer;
    std::vector<uint32_t> sparceMemory;
    std::vector<uint32_t> protectedMemory;
    std::vector<uint32_t> surface;
  };
public:
  explicit Device(VkInstance instance, VkSurfaceKHR surface);
  virtual ~Device();
  VkDevice Logical() const { return *ldevice_; }
  VkPhysicalDevice Physical() const { return pdevice_; }
  const std::vector<uint32_t>& Families() const { return families_; }
  const DeviceQueues& Queues() const { return queues_; }
  VkQueue GraphicsQueue() const { return graphicsQueue_; }
  VkQueue PresentQueue() const { return presentQueue_; }
  uint32_t GetMemoryType(uint32_t filter, VkMemoryPropertyFlags props) const;
private:
  void CreateDevice(uint32_t graphicsFamily, uint32_t surfaceFamily);
private:
  VkPhysicalDevice pdevice_;
  AutoDestroyer<VkDevice> ldevice_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
  std::vector<uint32_t> families_;
  DeviceQueues queues_;
  VkPhysicalDeviceMemoryProperties props_;
};

typedef std::unique_ptr<Device> DevicePtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_DEVICE

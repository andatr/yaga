#ifndef YAGA_VULKAN_RENDERER_SRC_DEVICE
#define YAGA_VULKAN_RENDERER_SRC_DEVICE

#include <functional>
#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>

#include "vulkan.h"
#include "assets/application.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

struct VulkanExtensions
{
  bool KHR_getMemoryRequirements2;
  bool KHR_getPhysicalDeviceProperties2;
  bool KHR_dedicatedAllocation;
  bool KHR_bindMemory2;
  bool EXT_memoryBudget;
  bool AMD_deviceCoherentMemory;
};

class Device : private boost::noncopyable
{
public:
  struct QueueFamilyIndices
  {
    uint32_t graphics;
    uint32_t surface;
    uint32_t compute;
    uint32_t transfer;
    // uint32_t sparceMemory;
    // uint32_t protectedMemory;
  };
  typedef std::function<void(VkCommandBuffer cmd)> CommandHandler;

public:
  explicit Device(VkInstance instance, VkSurfaceKHR surface, const VulkanExtensions& extensions);
  virtual ~Device();
  VkDevice                           operator*() const { return *logicalDevice_;  }
  VkPhysicalDevice                    physical() const { return physicalDevice_;  }
  const VulkanExtensions&           extensions() const { return extensions_;      }
  const VkPhysicalDeviceProperties& properties() const { return properties_;      }
  const QueueFamilyIndices&      queueFamilies() const { return queueFamilies_;   }
  VkCommandPool                    commandPool() const { return *commandPool_;    }
  VkDescriptorPool              descriptorPool() const { return *descriptorPool_; }
  VkQueue graphicsQueue() const { return queues_[0]; }
  VkQueue presentQueue()  const { return queues_[1]; }
  VkQueue transferQueue() const { return queues_[2]; }
  VkQueue computeQueue()  const { return queues_[3]; }
  void createDescriptorPool(uint32_t frames, const assets::Application* limits);
  uint32_t getMemoryType(uint32_t filter, VkMemoryPropertyFlags props) const;
  void submitCommand(const CommandHandler& command) const;
  void waitIdle() const;

private:
  void createDevice(std::vector<const char*>& extensions);
  void createCommandPool();
  void createImmediateCommand();

private:
  VkPhysicalDevice physicalDevice_;
  VkPhysicalDeviceProperties properties_;
  VkPhysicalDeviceMemoryProperties memoryProperties_;
  AutoDestructor<VkDevice> logicalDevice_;
  QueueFamilyIndices queueFamilies_;
  VkQueue queues_[4];
  AutoDestructor<VkCommandPool> commandPool_;
  AutoDestructor<VkCommandBuffer> immediateCommand_;
  VulkanExtensions extensions_;
  AutoDestructor<VkDescriptorPool> descriptorPool_;
};

typedef std::unique_ptr<Device> DevicePtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_DEVICE

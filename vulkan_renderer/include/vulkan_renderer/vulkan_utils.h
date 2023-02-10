#ifndef YAGA_VULKAN_RENDERER_VULKAN_UTILS
#define YAGA_VULKAN_RENDERER_VULKAN_UTILS

#include "vulkan_renderer/device.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"
#include "vulkan_renderer/window.h"
#include "engine/config.h"
#include "utility/auto_destructor.h"

#include <memory>
#include <vector>

namespace yaga {
namespace vk {

void initVulkan(
  ConfigPtr config,
  const SwapchainParams& swParams,
  AutoDestructor<VkInstance>& instance,
  WindowPtr& window,
  AutoDestructor<VkDebugUtilsMessengerEXT>& debugMessenger,
  DevicePtr& device,
  AutoDestructor<VmaAllocator>& allocator,
  SwapchainPtr& swapchain
);
VkPipelineShaderStageCreateInfo getShaderStageInfo(VkShaderModule module, VkShaderStageFlagBits type, const char* funcName);
AutoDestructor<VkSemaphore>           createSemaphore     (VkDevice device);
AutoDestructor<VkFence>               createFence         (VkDevice device);
VkDescriptorSet                       createDescriptor    (VkDevice device, const VkDescriptorSetAllocateInfo&     info);
std::vector<VkDescriptorSet>          createDescriptors   (VkDevice device, const VkDescriptorSetAllocateInfo&     info);
AutoDestructor<VkSampler>             createSampler       (VkDevice device, const VkSamplerCreateInfo&             info);
AutoDestructor<VkDescriptorSetLayout> createLayout        (VkDevice device, const VkDescriptorSetLayoutCreateInfo& info);
AutoDestructor<VkPipelineLayout>      createLayout        (VkDevice device, const VkPipelineLayoutCreateInfo&      info);
AutoDestructor<VkRenderPass>          createRenderPass    (VkDevice device, const VkRenderPassCreateInfo&          info);
AutoDestructor<VkShaderModule>        createShader        (VkDevice device, const VkShaderModuleCreateInfo&        info);
AutoDestructor<VkPipeline>            createPipeline      (VkDevice device, const VkGraphicsPipelineCreateInfo&    info);
AutoDestructor<VkPipeline>            createPipeline      (VkDevice device, const VkComputePipelineCreateInfo&     info);
AutoDestructor<VkCommandPool>         createCommandPool   (VkDevice device, const VkCommandPoolCreateInfo&         info);
AutoDestructor<VkDescriptorPool>      createDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo&      info);
AutoDestructor<VkImageView>           createImageView     (VkDevice device, const VkImageViewCreateInfo&           info);
AutoDestructor<VkCommandBuffer>       createCommand       (VkDevice device, VkCommandPool pool, const VkCommandBufferAllocateInfo& info);
void                                  updateDescriptor    (VkDevice device, const VkWriteDescriptorSet* writers, size_t count);
AutoDestructor<VkImage>               createImage         (VmaAllocator allocator, const VkImageCreateInfo& info);
void insertDescriptorPoolSize(std::vector<VkDescriptorPoolSize>& sizes, VkDescriptorType type, uint32_t count);
void copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandBuffer command);

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_VULKAN_UTILS

#ifndef YAGA_RENDERER_SRC_VIDEO_BUFFER
#define YAGA_RENDERER_SRC_VIDEO_BUFFER

#include <vector>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device.h"
#include "device_buffer.h"
#include "texture.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class VideoBuffer : private boost::noncopyable
{
public:
  VideoBuffer(Device* device, Allocator* allocator, VkSurfaceKHR surface, VkExtent2D size);
  virtual ~VideoBuffer();
  const VkFormat& ImageFormat() const { return format_; }
  const VkExtent2D& Size() const { return size_; }
  const std::vector<TexturePtr>& Textures() const { return textures_; }
  VkSwapchainKHR Swapchain() const { return *swapchain_; }
  VkPipelineLayout PipelineLayout() const { return *pipelineLayout_; }
  const std::vector<VkDescriptorSet>& DescriptorSets() const { return descriptorSets_; }
  void TmpUpdate(uint32_t index);
private:
  void CreateUniformBuffers(VkDevice device, Allocator* allocator);
  void CreateDescriptorPool(VkDevice device);
  void CreateDescriptorLayout(VkDevice device);
  void CreateDescriptorSets(VkDevice device);  
  void CreatePipelineLayout(VkDevice device);
private:
  VkFormat format_;
  VkExtent2D size_;
  AutoDestroyer<VkSwapchainKHR> swapchain_;
  std::vector<VkImage> images_;
  std::vector<TexturePtr> textures_;
  AutoDestroyer<VkPipelineLayout> pipelineLayout_;
  AutoDestroyer<VkDescriptorPool> descriptorPool_;
  AutoDestroyer<VkDescriptorSetLayout> descriptorSetLayout_;
  std::vector<VkDescriptorSet> descriptorSets_;
  std::vector<DeviceBufferPtr> uniformBuffers_;
};

typedef std::unique_ptr<VideoBuffer> VideoBufferPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_VIDEO_BUFFER

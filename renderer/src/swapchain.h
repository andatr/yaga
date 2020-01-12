#ifndef YAGA_RENDERER_SRC_VIDEO_BUFFER
#define YAGA_RENDERER_SRC_VIDEO_BUFFER

#include <vector>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device.h"
#include "device_buffer.h"
#include "image.h"
#include "image_view.h"
#include "utility/auto_destructor.h"

namespace yaga
{

class Swapchain : private boost::noncopyable
{
public:
  Swapchain(Device* device, Allocator* allocator, VkSurfaceKHR surface, VkExtent2D size);
  virtual ~Swapchain();
  const VkFormat& imageFormat() const { return format_; }
  const VkFormat& depthFormat() const { return depthFormat_; }
  const VkExtent2D& size() const { return size_; }
  VkSampleCountFlagBits msaa() const { return msaa_; }
  VkSampler textureSampler() const { return *textureSampler_; }
  const std::vector<ImageViewPtr>& frames() const { return imageViews_; }
  VkImageView renderTarget() const { return **targetImageView_; }
  VkImageView depthBuffer() const { return **depthImageView_; }
  VkSwapchainKHR operator*() const { return *swapchain_; }
  VkPipelineLayout pipelineLayout() const { return *pipelineLayout_; }
  VkDescriptorPool descriptorPool() const { return *descriptorPool_; }
  VkDescriptorSetLayout descriptorSetLayout() const { return *descriptorSetLayout_; }
  const std::vector<DeviceBufferPtr>& uniformBuffers() const { return uniformBuffers_; }
  void tmpUpdate(uint32_t index);
private:
  void createUniformBuffers();
  void createTextureSampler();
  void createDescriptorPool();
  void createDescriptorLayout();
  void createPipelineLayout();
  void createRenderTarget();
  void createDepthImage();
private:
  Device* device_;
  VkDevice vkDevice_;
  Allocator* allocator_;
  VkFormat format_;
  VkExtent2D size_;
  VkSampleCountFlagBits msaa_;
  AutoDestructor<VkSwapchainKHR> swapchain_;
  std::vector<std::unique_ptr<Image>> images_;
  std::vector<ImageViewPtr> imageViews_;
  AutoDestructor<VkPipelineLayout> pipelineLayout_;
  AutoDestructor<VkDescriptorPool> descriptorPool_;
  AutoDestructor<VkDescriptorSetLayout> descriptorSetLayout_;
  std::vector<DeviceBufferPtr> uniformBuffers_;
  AutoDestructor<VkSampler> textureSampler_;
  std::unique_ptr<Image> targetImage_;
  std::unique_ptr<ImageView> targetImageView_;
  VkFormat depthFormat_;
  std::unique_ptr<Image> depthImage_;
  std::unique_ptr<ImageView> depthImageView_;
};

typedef std::unique_ptr<Swapchain> SwapchainPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_VIDEO_BUFFER

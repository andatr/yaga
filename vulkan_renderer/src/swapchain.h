#ifndef YAGA_VULKAN_RENDERER_SRC_VIDEO_BUFFER
#define YAGA_VULKAN_RENDERER_SRC_VIDEO_BUFFER

#include <vector>
#include <boost/noncopyable.hpp>

#include "device.h"
#include "image.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Swapchain : private boost::noncopyable
{
public:
  Swapchain(Device* device, VmaAllocator allocator, VkSurfaceKHR surface, VkExtent2D resolution);
  virtual ~Swapchain();
  VkSwapchainKHR operator*() const { return *swapchain_; }
  const VkExtent2D& resolution() const { return resolution_; }
  VkSampleCountFlagBits msaa() const { return msaa_; }
  VkRenderPass renderPass() const { return *renderPass_; }
  const std::vector<VkFramebuffer>& frameBuffers() const { return frameBufferRefs_; }

private:
  VkFormat createSwapchain(VkSurfaceKHR surface, VkExtent2D resolution);
  void createFrameImages(VkFormat format);
  void createRenderPass(VkFormat format);
  void createFramebuffers();

private:
  Device* device_;
  VkDevice vkDevice_;
  VmaAllocator allocator_;
  VkExtent2D resolution_;
  VkSampleCountFlagBits msaa_;
  AutoDestructor<VkSwapchainKHR> swapchain_;
  std::vector<AutoDestructor<VkImageView>> frameImages_;
  ImagePtr renderTarget_;
  ImagePtr depthImage_;
  AutoDestructor<VkRenderPass> renderPass_;
  std::vector<AutoDestructor<VkFramebuffer>> frameBuffers_;
  std::vector<VkFramebuffer> frameBufferRefs_;
};

typedef std::unique_ptr<Swapchain> SwapchainPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_VIDEO_BUFFER

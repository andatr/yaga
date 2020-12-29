#ifndef YAGA_VULKAN_RENDERER_SRC_VIDEO_BUFFER
#define YAGA_VULKAN_RENDERER_SRC_VIDEO_BUFFER

#include <limits>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/signals2.hpp>

#include "device.h"
#include "image.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Swapchain : private boost::noncopyable
{
public:
  static const uint32_t BAD_IMAGE = std::numeric_limits<uint32_t>::max();
  typedef boost::signals2::connection Connection;
  typedef boost::signals2::signal<void(void)> SignalResize;

public:
  Swapchain(Device* device, VmaAllocator allocator, VkSurfaceKHR surface, VkExtent2D resolution);
  virtual ~Swapchain();
  void resize(VkExtent2D resolution);
  Device*               device() const { return device_;             }
  VkSwapchainKHR     operator*() const { return *swapchain_;         }
  VkFormat         imageFormat() const { return info_.imageFormat;   }
  VkFormat         depthFormat() const { return depthFormat_;        }
  VkSampleCountFlagBits   msaa() const { return msaa_;               }
  const VkExtent2D& resolution() const { return info_.imageExtent;   }
  uint32_t          imageCount() const { return static_cast<uint32_t>(images_.size()); }
  VkImageView      depthBuffer() const { return depthImage_->view(); }
  VkImageView       msaaBuffer() const { return msaaImage_->view();  }
  VkImageView    image(size_t i) const { return *images_[i];         }
  const VkSwapchainCreateInfoKHR& info() const { return info_; }
  uint32_t acquireImage(VkSemaphore semaphore);
  bool presentImage(VkSemaphore semaphore, uint32_t image);
  Connection onResize(const SignalResize::slot_type& handler) { return sigResize_.connect(handler); }
  void onResize(const Connection& handler) { return sigResize_.disconnect(handler); }

private:
  void init();
  void createSwapchain();
  void createImageViews();

private:
  Device* device_;
  VmaAllocator allocator_;
  VkSurfaceKHR surface_;
  VkSwapchainCreateInfoKHR info_;
  VkFormat depthFormat_;
  VkSampleCountFlagBits msaa_;
  AutoDestructor<VkSwapchainKHR> swapchain_;
  ImagePtr msaaImage_;
  ImagePtr depthImage_;
  std::vector<AutoDestructor<VkImageView>> images_;
  SignalResize sigResize_;
};

typedef std::unique_ptr<Swapchain> SwapchainPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_VIDEO_BUFFER

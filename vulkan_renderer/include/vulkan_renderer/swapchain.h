#ifndef YAGA_VULKAN_RENDERER_SWAPCHAIN
#define YAGA_VULKAN_RENDERER_SWAPCHAIN

#include "utility/compiler.h"

#include <limits>
#include <vector>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "vulkan_renderer/device.h"
#include "vulkan_renderer/image.h"
#include "vulkan_renderer/image_view.h"
#include "vulkan_renderer/vulkan.h"
#include "engine/config.h"
#include "utility/auto_destructor.h"
#include "utility/signal.h"

namespace yaga {
namespace vk {

struct SwapchainParams
{
  VkImageUsageFlagBits imageUsage;
};

class Swapchain : private boost::noncopyable
{
public:
  static const uint32_t BAD_IMAGE = std::numeric_limits<uint32_t>::max();
  typedef boost::signals2::signal<void(void)> SignalResize;

public:
  Swapchain(Device* device, VmaAllocator allocator, VkSurfaceKHR surface, ConfigPtr config, const SwapchainParams& params);
  virtual ~Swapchain();
  void resize(VkExtent2D resolution);
  const VkSurfaceCapabilitiesKHR& capabilitues() const { return capabilitues_; }
  Device*                  device() const { return device_;             }
  VkSwapchainKHR        operator*() const { return *swapchain_;         }
  VkFormat            imageFormat() const { return info_.imageFormat;   }
  VkFormat            depthFormat() const { return depthFormat_;        }
  VkSampleCountFlagBits msaaLevel() const { return msaaLevel_;          }
  const VkExtent2D&    resolution() const { return info_.imageExtent;   }
  uint32_t             imageCount() const { return static_cast<uint32_t>(images_.size()); }
  VkImageView         depthBuffer() const { return depthImage_->view(); }
  VkImageView          msaaBuffer() const { return msaaImage_->view();  }
  ImageView         image(size_t i) const { return images_[i];          }
  const std::vector<ImageView>& images() const { return images_;        }
  const VkSwapchainCreateInfoKHR& info() const { return info_; }
  uint32_t acquireImage(VkSemaphore semaphore);
  bool presentImage(VkSemaphore semaphore, uint32_t image);
  SignalConnectionPtr onResize(const SignalResize::slot_type& handler);

private:
  VkSwapchainCreateInfoKHR getSwapchainInfo(ConfigPtr config, const SwapchainParams& params);
  void init();
  void createSwapchain();
  void createImageViews();

private:
  Device* device_;
  VmaAllocator allocator_;
  VkSurfaceKHR surface_;
  VkSwapchainCreateInfoKHR info_;
  VkSurfaceCapabilitiesKHR capabilitues_;
  VkFormat depthFormat_;
  VkSampleCountFlagBits msaaLevel_;
  AutoDestructor<VkSwapchainKHR> swapchain_;
  ImagePtr msaaImage_;
  ImagePtr depthImage_;
  std::vector<ImageView> images_;
  std::vector<AutoDestructor<VkImageView>> imagesViews_;
  SignalResize sigResize_;
};

typedef std::unique_ptr<Swapchain> SwapchainPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SWAPCHAIN

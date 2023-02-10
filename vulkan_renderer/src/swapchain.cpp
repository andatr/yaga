#include "precompiled.h"
#include "vulkan_renderer/swapchain.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
VkSurfaceFormatKHR pickColorFormat(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  std::vector<VkSurfaceFormatKHR> formats;
  uint32_t count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
  if (count == 0) {
    THROW("Could not choose color format");
  }
  formats.resize(count);
  VULKAN_GUARD(
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, formats.data()), "Could not enumerate color format");

  auto format = std::find_if(formats.begin(), formats.end(), [](const auto& f) {
    // TODO: uncomment
    // VkFormatProperties props;
    // vkGetPhysicalDeviceFormatProperties(device, f, &props);
    return /*props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT*/
      f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  });

  // If the surface format list only includes one entry with
  // VK_FORMAT_UNDEFINED, there is no preferred format.
  // Assume VK_FORMAT_B8G8R8A8_RGB.
  // TODO Consider passing in desired format from app.
  /*if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
  {
    colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
  }
  else
  {
    assert(formatCount >= 1);
    uint32_t i;
    for (i = 0; i < formatCount; i++) {
      if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) {
        break;
      }
    }
    if (i == formatCount) {
      // Pick the first available, if no SRGB.
      // FIXME probably should raise an error...
      i = 0;
    }
    colorFormat = surfaceFormats[i].format;
  }
  colorSpace = surfaceFormats[0].colorSpace;
  return true;*/

  if (format == formats.end()) {
    THROW("Could not choose color format");
  }
  return *format;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPresentModeKHR pickBufferingMode(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  std::vector<VkPresentModeKHR> modes;
  uint32_t count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
  if (count == 0) {
    THROW("Could not choose buffering mode");
  }
  modes.resize(count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, modes.data());
  return std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) == modes.end()
    ? VK_PRESENT_MODE_FIFO_KHR
    : VK_PRESENT_MODE_MAILBOX_KHR;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkExtent2D pickResolution(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D requested)
{
  // if currentExtent is not MAX_UINT32 we must use values provided by Vulkan
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }
  // init with min supported values
  VkExtent2D resolution = { 
    std::min(capabilities.maxImageExtent.width,  requested.width),
    std::min(capabilities.maxImageExtent.height, requested.height)
  };
  // update to max supported
  resolution.width  = std::max(resolution.width,  capabilities.minImageExtent.width);
  resolution.height = std::max(resolution.height, capabilities.minImageExtent.height);
  return resolution;
}

// -----------------------------------------------------------------------------------------------------------------------------
uint32_t pickBufferCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
  if (capabilities.maxImageCount == 0) {
    THROW("Could not choose buffering mode");
  }
  return std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);
}

// -----------------------------------------------------------------------------------------------------------------------------
VkFormat getDepthImageFormat(
  VkPhysicalDevice device,
  const std::vector<VkFormat>& candidates,
  VkImageTiling tiling,
  VkFormatFeatureFlags features)
{
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(device, format, &props);
    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    }
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  THROW("Could not find supported depth image format");
}

// -----------------------------------------------------------------------------------------------------------------------------
ImagePtr createImage(
  Device* device,
  VmaAllocator allocator,
  VkExtent2D resolution,
  VkFormat format,
  VkSampleCountFlagBits msaa,
  VkImageUsageFlags usage,
  VkImageAspectFlagBits aspectMask)
{
  VkImageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType     = VK_IMAGE_TYPE_2D;
  info.extent.width  = resolution.width;
  info.extent.height = resolution.height;
  info.extent.depth  = 1;
  // info.mipLevels = std::floor(std::log2(std::max(asset->image()->width(), asset->image()->height())))) + 1;
  info.mipLevels     = 1;
  info.arrayLayers   = 1;
  info.format        = format;
  info.tiling        = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage         = usage;
  info.samples       = msaa;
  info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format   = info.format;
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.subresourceRange.aspectMask     = aspectMask;
  viewInfo.subresourceRange.baseMipLevel   = 0;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount     = 1;
  viewInfo.subresourceRange.levelCount     = info.mipLevels;

  return std::make_unique<Image>(device, allocator, info, viewInfo);
}

// -----------------------------------------------------------------------------------------------------------------------------
VkSampleCountFlagBits getMaxMsaaLevel(Device* device)
{
  auto msaa = device->properties().limits.framebufferColorSampleCounts &
              device->properties().limits.framebufferDepthSampleCounts;
  if (msaa & VK_SAMPLE_COUNT_64_BIT) {
    return VK_SAMPLE_COUNT_64_BIT;
  }
  if (msaa & VK_SAMPLE_COUNT_32_BIT) {
    return VK_SAMPLE_COUNT_32_BIT;
  }
  if (msaa & VK_SAMPLE_COUNT_16_BIT) {
    return VK_SAMPLE_COUNT_16_BIT;
  }
  if (msaa & VK_SAMPLE_COUNT_8_BIT) {
    return VK_SAMPLE_COUNT_8_BIT;
  }
  if (msaa & VK_SAMPLE_COUNT_4_BIT) {
    return VK_SAMPLE_COUNT_4_BIT;
  }
  if (msaa & VK_SAMPLE_COUNT_2_BIT) {
    return VK_SAMPLE_COUNT_2_BIT;
  }
  return VK_SAMPLE_COUNT_1_BIT;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Swapchain::Swapchain(
  Device* device,
  VmaAllocator allocator,
  VkSurfaceKHR surface,
  ConfigPtr config,
  const SwapchainParams& params
) :
  device_(device),
  allocator_(allocator),
  surface_(surface),
  info_{},
  capabilitues_{},
  depthFormat_(VK_FORMAT_UNDEFINED),
  msaaLevel_((VkSampleCountFlagBits)0)
{
  info_ = getSwapchainInfo(config, params);
  if (config->rendering().depthBuffer()) {
    depthFormat_ = getDepthImageFormat(device_->physical(),
      { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  }
  if (config->rendering().msaaLevel()) {
    msaaLevel_ = getMaxMsaaLevel(device);  
  }
  init();
}

// -----------------------------------------------------------------------------------------------------------------------------
Swapchain::~Swapchain()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Swapchain::resize(VkExtent2D resolution)
{
  if (resolution.height == 0 || resolution.width == 0) return;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_->physical(), surface_, &capabilitues_);
  info_.imageExtent = pickResolution(capabilitues_, resolution);
  init();
  sigResize_();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Swapchain::init()
{
  createSwapchain();
  if (msaaLevel_) {
    msaaImage_ = createImage(device_, allocator_, info_.imageExtent, info_.imageFormat, msaaLevel_,
      VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      VK_IMAGE_ASPECT_COLOR_BIT);
  }
  if (depthFormat_) {
    depthImage_ = createImage(device_, allocator_, info_.imageExtent, depthFormat_, msaaLevel_,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
  }
  createImageViews();
}

// -----------------------------------------------------------------------------------------------------------------------------
VkSwapchainCreateInfoKHR Swapchain::getSwapchainInfo(ConfigPtr config, const SwapchainParams& params)
{
  auto pdevice = device_->physical();
  auto colorFormat = pickColorFormat(pdevice, surface_);
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pdevice, surface_, &capabilitues_);

  VkExtent2D resolution { config->window().width(), config->window().height() };
  VkSwapchainCreateInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.surface          = surface_;
  info.oldSwapchain     = VK_NULL_HANDLE;
  info.minImageCount    = pickBufferCount(capabilitues_);
  info.imageFormat      = colorFormat.format;
  info.imageColorSpace  = colorFormat.colorSpace;
  info.imageExtent      = pickResolution(capabilitues_, resolution);
  info.imageArrayLayers = 1;
  info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  if (config->rendering().msaaLevel()) {
    info.imageUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;
  }
  info.imageUsage    |= params.imageUsage;
  info.preTransform   = capabilitues_.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode    = pickBufferingMode(pdevice, surface_);
  info.clipped        = VK_TRUE;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
uint32_t Swapchain::acquireImage(VkSemaphore semaphore)
{
  uint32_t image;
  auto result = vkAcquireNextImageKHR(**device_, *swapchain_, INT64_MAX, semaphore, VK_NULL_HANDLE, &image);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    return BAD_IMAGE;
  }
  if (result != VK_SUCCESS) {
    THROW("Failed to acquire swapchain image");
  }
  return image;
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Swapchain::presentImage(VkSemaphore semaphore, uint32_t image)
{
  const auto swapchain = *swapchain_;
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = &semaphore;
  presentInfo.swapchainCount     = 1;
  presentInfo.pSwapchains        = &swapchain;
  presentInfo.pImageIndices      = &image;
  auto result = vkQueuePresentKHR(device_->presentQueue(), &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    return false;
  }
  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    THROW("Could not present an image");
  }
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Swapchain::createSwapchain()
{
  const auto& queueFamilies = device_->queueFamilies();
  std::array<uint32_t, 2> families = { queueFamilies.graphics, queueFamilies.surface };
  if (queueFamilies.graphics == queueFamilies.surface) {
    info_.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
  else {
    info_.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    info_.queueFamilyIndexCount = static_cast<uint32_t>(families.size());
    info_.pQueueFamilyIndices   = families.data();
  }

  auto destroySwapchain = [device = **device_](auto chain) {
    vkDestroySwapchainKHR(device, chain, nullptr);
    LOG(trace) << "Swapchain destroyed";
  };

  swapchain_.set();
  VkSwapchainKHR swapchain;
  VULKAN_GUARD(vkCreateSwapchainKHR(**device_, &info_, nullptr, &swapchain), "Could not create Swapchain");
  swapchain_.set(swapchain, destroySwapchain);
  LOG(trace) << "Swapchain created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void Swapchain::createImageViews()
{
  auto destroyImageView = [device = **device_](auto imageView) {
    vkDestroyImageView(device, imageView, nullptr);
    LOG(trace) << "Vulkan Image View destroyed";
  };

  VkImageViewCreateInfo info{};
  info.sType        = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.format       = info_.imageFormat;
  info.viewType     = VK_IMAGE_VIEW_TYPE_2D;
  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  info.subresourceRange.baseMipLevel   = 0;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount     = 1;
  info.subresourceRange.levelCount     = 1;
   
  uint32_t imageCount = 0;
  vkGetSwapchainImagesKHR(**device_, *swapchain_, &imageCount, nullptr);
  std::vector<VkImage> images(imageCount);
  images_.resize(imageCount);
  imagesViews_.resize(imageCount);
  vkGetSwapchainImagesKHR(**device_, *swapchain_, &imageCount, images.data());
  for (uint32_t i = 0; i < imageCount; ++i) {
    info.image = images[i];
    VkImageView imageView;
    VULKAN_GUARD(vkCreateImageView(**device_, &info, nullptr, &imageView), "Could not create Vulkan Image View");
    imagesViews_[i].set(imageView, destroyImageView);
    images_[i] = { images[i], imageView };
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnectionPtr Swapchain::onResize(const SignalResize::slot_type& handler)
{
  return std::make_unique<SignalConnection>(sigResize_.connect(handler));
}

} // !namespace vk
} // !namespace yaga
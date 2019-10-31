#include "precompiled.h"
#include "video_buffer.h"
#include "device.h"
#include "texture.h"

namespace yaga
{
namespace
{

// -------------------------------------------------------------------------------------------------------------------------
VkSurfaceFormatKHR PickColorFormat(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  std::vector<VkSurfaceFormatKHR> formats;

  uint32_t count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
  if (count == 0) {
    THROW("Could not choose color format");
  }
  formats.resize(count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, formats.data());
      
  auto format = std::find_if(formats.begin(), formats.end(), [](const auto& f) {
    return f.format == VK_FORMAT_B8G8R8A8_UNORM &&
      f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  });
  return format == formats.end() ? formats[0] : *format;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPresentModeKHR PickBufferingMode(VkPhysicalDevice device, VkSurfaceKHR surface)
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

// -------------------------------------------------------------------------------------------------------------------------
VkExtent2D PickResolution(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D requested)
{
  // if currentExtent is not MAX_UINT32 we must use values provided by Vulkan
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }

  // init with min supported values
  VkExtent2D resolution = {
    std::min(capabilities.maxImageExtent.width, requested.width),
    std::min(capabilities.maxImageExtent.height, requested.height)
  };

  // update to max supported
  resolution.width  = std::max(resolution.width,  capabilities.minImageExtent.width);
  resolution.height = std::max(resolution.height, capabilities.minImageExtent.height);
  return resolution;
}

// -------------------------------------------------------------------------------------------------------------------------
uint32_t PickBufferCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
  if (capabilities.maxImageCount == 0) {
    THROW("Could not choose buffering mode");
  }
  return std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);
}

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
VideoBuffer::VideoBuffer(Device* device, VkSurfaceKHR surface, VkExtent2D resolution)
{
  const auto colorFormat = PickColorFormat(device->Physical(), surface);
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->Physical(), surface, &capabilities);

  VkSwapchainCreateInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.surface = surface;
  info.oldSwapchain = VK_NULL_HANDLE;
  info.minImageCount = PickBufferCount(capabilities);
  info.imageFormat = colorFormat.format;
  info.imageColorSpace = colorFormat.colorSpace;
  info.imageExtent = PickResolution(capabilities, resolution);
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  format_ = info.imageFormat;
  resolution_ = info.imageExtent;

  const auto& families = device->Families();
  if (families.size() > 1) {
    info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    info.queueFamilyIndexCount = static_cast<uint32_t>(families.size());
    info.pQueueFamilyIndices = families.data();
  }
  else {
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  info.preTransform = capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = PickBufferingMode(device->Physical(), surface);
  info.clipped = VK_TRUE;

  auto logicalDeivce = device->Logical();
  auto destroySwapchain = [logicalDeivce](auto chain) {
    vkDestroySwapchainKHR(logicalDeivce, chain, nullptr);
    LOG(trace) << "Swaprecompiledain deleted";
  };
  VkSwapchainKHR chain;
  if (vkCreateSwapchainKHR(logicalDeivce, &info, nullptr, &chain) != VK_SUCCESS) {
    THROW("Could not create Swaprecompiledain");
  }
	swapchain_.Assign(chain, destroySwapchain);
  LOG(trace) << "Swaprecompiledain created";

  uint32_t imageCount;
  vkGetSwapchainImagesKHR(logicalDeivce, *swapchain_, &imageCount, nullptr);
  images_.resize(imageCount);
  textures_.resize(imageCount);
  vkGetSwapchainImagesKHR(logicalDeivce, *swapchain_, &imageCount, images_.data());
  for (uint32_t i = 0; i < imageCount; ++i) {
    textures_[i] = std::make_unique<Texture>(logicalDeivce, images_[i], colorFormat.format);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
VideoBuffer::~VideoBuffer()
{
}

} // !namespace yaga
#include "precompiled.h"
#include "swapchain.h"
#include "engine/vertex.h"

namespace yaga
{
namespace vk
{
namespace
{

// -------------------------------------------------------------------------------------------------------------------------
VkSurfaceFormatKHR pickColorFormat(VkPhysicalDevice device, VkSurfaceKHR surface)
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
    // TODO: uncomment
    //VkFormatProperties props;
    //vkGetPhysicalDeviceFormatProperties(device, f, &props);
    return /*props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT; &&*/
      f.format == VK_FORMAT_B8G8R8A8_UNORM &&
      f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  });
  if (format == formats.end()) {
    THROW("Could not choose color format");
  }
  return *format;
}

// -------------------------------------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------------------------------------
VkExtent2D pickResolution(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D requested)
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
uint32_t pickBufferCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
  if (capabilities.maxImageCount == 0) {
    THROW("Could not choose buffering mode");
  }
  return std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);
}

// -------------------------------------------------------------------------------------------------------------------------
VkFormat getDepthImageFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling,
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

// -------------------------------------------------------------------------------------------------------------------------
VkSampleCountFlagBits getMaxMsaaLevel(Device* device)
{
  auto msaa = device->properties().limits.framebufferColorSampleCounts &
    device->properties().limits.framebufferDepthSampleCounts;
  if (msaa & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
  if (msaa & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
  if (msaa & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
  if (msaa & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT;  }
  if (msaa & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT;  }
  if (msaa & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT;  }
  return VK_SAMPLE_COUNT_1_BIT;
}

// -------------------------------------------------------------------------------------------------------------------------
ImagePtr createImage(Device* device, VmaAllocator allocator, VkExtent2D resolution, VkFormat format,
  VkSampleCountFlagBits msaa, VkImageUsageFlags usage, VkImageAspectFlagBits aspectMask)
{
  VkImageCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.extent.width = resolution.width;
  info.extent.height = resolution.height;
  info.extent.depth = 1;
  // info.mipLevels = std::floor(std::log2(std::max(asset->image()->width(), asset->image()->height())))) + 1;
  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.format = format;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage = usage;
  info.samples = msaa;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkImageViewCreateInfo viewInfo {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.subresourceRange.aspectMask = aspectMask;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VmaAllocationCreateInfo allocInfo {};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  return std::make_unique<Image>(device, allocator, info, viewInfo, allocInfo);
}

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
Swapchain::Swapchain(Device* device, VmaAllocator allocator, VkSurfaceKHR surface, VkExtent2D resolution) :
  device_(device), vkDevice_(**device), allocator_(allocator), resolution_(resolution)
{
  msaa_ = getMaxMsaaLevel(device);
  auto format = createSwapchain(surface, resolution);
  renderTarget_ = createImage(device_, allocator_, resolution_, format, msaa_,
    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_IMAGE_ASPECT_COLOR_BIT);
  auto depthFormat = getDepthImageFormat(
    device_->physical(),
    { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
    VK_IMAGE_TILING_OPTIMAL,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
  );
  depthImage_ = createImage(device_, allocator_, resolution_, depthFormat, msaa_,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_IMAGE_ASPECT_DEPTH_BIT);
  createRenderPass(format);
  createFrameImages(format);
  createFramebuffers();
}

// -------------------------------------------------------------------------------------------------------------------------
Swapchain::~Swapchain()
{
}

// -------------------------------------------------------------------------------------------------------------------------
VkFormat Swapchain::createSwapchain(VkSurfaceKHR surface, VkExtent2D resolution)
{
  auto pdevice = device_->physical();
  auto colorFormat = pickColorFormat(pdevice, surface);
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pdevice, surface, &capabilities);

  VkSwapchainCreateInfoKHR info {};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.surface = surface;
  info.oldSwapchain = VK_NULL_HANDLE;
  info.minImageCount = pickBufferCount(capabilities);
  info.imageFormat = colorFormat.format;
  info.imageColorSpace = colorFormat.colorSpace;
  info.imageExtent = pickResolution(capabilities, resolution);
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  info.preTransform = capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = pickBufferingMode(pdevice, surface);
  info.clipped = VK_TRUE;
  
  const auto& queueFamilies = device_->queueFamilies();
  if (queueFamilies.graphics == queueFamilies.surface) {
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
  else {
    uint32_t queueFamiliesArray[] = { queueFamilies.graphics, queueFamilies.surface };
    info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    info.queueFamilyIndexCount = 2;
    info.pQueueFamilyIndices = queueFamiliesArray;
  }

  auto destroySwapchain = [device = vkDevice_](auto chain) {
    vkDestroySwapchainKHR(device, chain, nullptr);
    LOG(trace) << "Swapchain destroyed";
  };
  VkSwapchainKHR chain;
  VULKAN_GUARD(vkCreateSwapchainKHR(vkDevice_, &info, nullptr, &chain), "Could not create Swapchain");
  swapchain_.set(chain, destroySwapchain);
  LOG(trace) << "Swapchain created";
  resolution_ = info.imageExtent;
  return colorFormat.format;
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::createFrameImages(VkFormat format)
{
  auto destroyImageView = [device = vkDevice_](auto imageView) {
    vkDestroyImageView(device, imageView, nullptr);
    LOG(trace) << "Vulkan Image View destroyed";
  };

  VkImageViewCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  info.subresourceRange.baseMipLevel = 0;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount = 1;
  info.format = format;
  info.subresourceRange.levelCount = 1;

  uint32_t imageCount;
  vkGetSwapchainImagesKHR(vkDevice_, *swapchain_, &imageCount, nullptr);
  std::vector<VkImage> images(imageCount);
  vkGetSwapchainImagesKHR(vkDevice_, *swapchain_, &imageCount, images.data());
  frameImages_.resize(imageCount);
  for (uint32_t i = 0; i < imageCount; ++i) {
    info.image = images[i];
    VkImageView imageView;
    VULKAN_GUARD(vkCreateImageView(vkDevice_, &info, nullptr, &imageView), "Could not create Vulkan Image View");
    frameImages_[i].set(imageView, destroyImageView);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::createRenderPass(VkFormat format)
{
  VkAttachmentDescription colorAttachment {};
  colorAttachment.format = format;
  colorAttachment.samples = msaa_;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment {};
  depthAttachment.format = depthImage_->info().format;
  depthAttachment.samples = msaa_;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription msaaAttachment {};
  msaaAttachment.format = format;
  msaaAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  msaaAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  msaaAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  msaaAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  msaaAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  msaaAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  msaaAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference msaaAttachmentRef {};
  msaaAttachmentRef.attachment = 2;
  msaaAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  subpass.pResolveAttachments = &msaaAttachmentRef;

  VkSubpassDependency dependency {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, msaaAttachment };
  VkRenderPassCreateInfo renderPassInfo {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  auto destroyRenderPass = [device = vkDevice_](auto renderPass) {
    vkDestroyRenderPass(device, renderPass, nullptr);
    LOG(trace) << "Render Pass destroyed";
  };
  VkRenderPass renderPass;
  VULKAN_GUARD(vkCreateRenderPass(vkDevice_, &renderPassInfo, nullptr, &renderPass), "Could not create Render Pass");
  renderPass_.set(renderPass, destroyRenderPass);
  LOG(trace) << "Render Pass created";
}

// ------------------------------------------------------------------------------------------------------------------------
void Swapchain::createFramebuffers()
{
  auto destroyFrameBuffer = [device = vkDevice_](auto frameBuffer) {
    vkDestroyFramebuffer(device, frameBuffer, nullptr);
    LOG(trace) << "Framebuffer destroyed";
  };
  frameBuffers_.resize(frameImages_.size());
  frameBufferRefs_.resize(frameImages_.size());
  for (size_t i = 0; i < frameImages_.size(); i++) {
    std::array<VkImageView, 3> attachments = { renderTarget_->view(), depthImage_->view(), *frameImages_[i] };
    VkFramebufferCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = *renderPass_;
    info.attachmentCount = static_cast<uint32_t>(attachments.size());
    info.pAttachments = attachments.data();
    info.width = resolution_.width;
    info.height = resolution_.height;
    info.layers = 1;
    VkFramebuffer frameBuffer;
    VULKAN_GUARD(vkCreateFramebuffer(vkDevice_, &info, nullptr, &frameBuffer), "Could not create Framebuffer");
    frameBuffers_[i].set(frameBuffer, destroyFrameBuffer);
    frameBufferRefs_[i] = frameBuffer;
  }
}

} // !namespace vk
} // !namespace yaga
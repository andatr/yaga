#include "precompiled.h"
#include "swapchain.h"
#include "asset/vertex.h"

namespace yaga
{
namespace
{

constexpr auto uniBufferSize = sizeof(UniformObject);

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
    return f.format == VK_FORMAT_B8G8R8A8_UNORM &&
      f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  });
  return format == formats.end() ? formats[0] : *format;
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
VkSampleCountFlagBits getMaxMsaaLevel(VkPhysicalDevice device)
{
  VkPhysicalDeviceProperties props;
  vkGetPhysicalDeviceProperties(device, &props);

 /* auto msaa = props.limits.framebufferColorSampleCounts & props.limits.framebufferDepthSampleCounts;
  if (msaa & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
  if (msaa & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
  if (msaa & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
  if (msaa & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT;  }
  if (msaa & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT;  }
  if (msaa & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT;  }*/
  return VK_SAMPLE_COUNT_1_BIT;
}

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
Swapchain::Swapchain(Device* device, Allocator* allocator, VkSurfaceKHR surface, VkExtent2D size) :
  device_(device), vkDevice_(**device), allocator_(allocator)
{
  auto pdevice = device->physical();
  msaa_ = getMaxMsaaLevel(pdevice);
  const auto colorFormat = pickColorFormat(pdevice, surface);
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pdevice, surface, &capabilities);

  VkSwapchainCreateInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.surface = surface;
  info.oldSwapchain = VK_NULL_HANDLE;
  info.minImageCount = pickBufferCount(capabilities);
  info.imageFormat = colorFormat.format;
  info.imageColorSpace = colorFormat.colorSpace;
  info.imageExtent = pickResolution(capabilities, size);
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  format_ = info.imageFormat;
  size_ = info.imageExtent;

  const auto& queueFamilies = device->queueFamilies();
  if (queueFamilies.graphics == queueFamilies.surface) {
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
  else {
    uint32_t queueFamiliesArray[] = { queueFamilies.graphics, queueFamilies.surface };
    info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    info.queueFamilyIndexCount = 2;
    info.pQueueFamilyIndices = queueFamiliesArray;
  }

  info.preTransform = capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = pickBufferingMode(pdevice, surface);
  info.clipped = VK_TRUE;

  auto destroySwapchain = [device = vkDevice_](auto chain) {
    vkDestroySwapchainKHR(device, chain, nullptr);
    LOG(trace) << "Swapchain destroyed";
  };
  VkSwapchainKHR chain;
  if (vkCreateSwapchainKHR(vkDevice_, &info, nullptr, &chain) != VK_SUCCESS) {
    THROW("Could not create Swapchain");
  }
  swapchain_.set(chain, destroySwapchain);
  LOG(trace) << "Swapchain created";

  uint32_t imageCount;
  vkGetSwapchainImagesKHR(vkDevice_, *swapchain_, &imageCount, nullptr);
  std::vector<VkImage> images(imageCount);
  vkGetSwapchainImagesKHR(vkDevice_, *swapchain_, &imageCount, images.data());
  images_.resize(imageCount);
  imageViews_.resize(imageCount);
  for (uint32_t i = 0; i < imageCount; ++i) {
    images_[i] = std::make_unique<Image>(images[i], colorFormat.format);
    imageViews_[i] = std::make_unique<ImageView>(device_, images_[i].get(), VK_IMAGE_ASPECT_COLOR_BIT);
  }

  createUniformBuffers();
  createTextureSampler();
  createDescriptorPool();
  createDescriptorLayout();
  createPipelineLayout();
  createRenderTarget();
  createDepthImage();
}

// -------------------------------------------------------------------------------------------------------------------------
Swapchain::~Swapchain()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::createDescriptorPool()
{
  std::array<VkDescriptorPoolSize, 2> poolSizes = {};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(images_.size());
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(images_.size());

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(images_.size());

  VkDescriptorPool pool;
  auto destroyPool = [device = vkDevice_](auto pool) {
    vkDestroyDescriptorPool(device, pool, nullptr);
    LOG(trace) << "Descriptor Pool destroyed";
  };
  if (vkCreateDescriptorPool(vkDevice_, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
    throw std::runtime_error("Could not create descriptor pool");
  }
  descriptorPool_.set(pool, destroyPool);
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::createPipelineLayout()
{
  VkPipelineLayoutCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = 1;
  info.pushConstantRangeCount = 0;
  info.pSetLayouts = &*descriptorSetLayout_;

  auto destroyLayout = [device = vkDevice_](auto layout) {
    vkDestroyPipelineLayout(device, layout, nullptr);
    LOG(trace) << "Pipeline Layout destroyed";
  };
  VkPipelineLayout layout;
  if (vkCreatePipelineLayout(vkDevice_, &info, nullptr, &layout) != VK_SUCCESS) {
    THROW("Could not create Pipeline Layout");
  }
  pipelineLayout_.set(layout, destroyLayout);
  LOG(trace) << "Pipeline Layout created";
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::createDescriptorLayout()
{
  VkDescriptorSetLayoutBinding uboBinding = {};
  uboBinding.binding = 0;
  uboBinding.descriptorCount = 1;
  uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboBinding.pImmutableSamplers = nullptr;
  uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerBinding = {};
  samplerBinding.binding = 1;
  samplerBinding.descriptorCount = 1;
  samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerBinding.pImmutableSamplers = nullptr;
  samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboBinding, samplerBinding };
  VkDescriptorSetLayoutCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = static_cast<uint32_t>(bindings.size());
  info.pBindings = bindings.data();

  auto destroyLayout = [device = vkDevice_](auto layout) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    LOG(trace) << "Descriptor Set Layout destroyed";
  };
  VkDescriptorSetLayout layout;
  if (vkCreateDescriptorSetLayout(vkDevice_, &info, nullptr, &layout) != VK_SUCCESS) {
    throw std::runtime_error("Could not create Descriptor Set Layout!");
  }
  descriptorSetLayout_.set(layout, destroyLayout);
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::createUniformBuffers()
{
  uniformBuffers_.resize(images_.size());
  for (size_t i = 0; i < images_.size(); i++) {
    uniformBuffers_[i] = std::make_unique<DeviceBuffer>(device_, allocator_, uniBufferSize,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::createTextureSampler()
{
  VkSamplerCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.magFilter = VK_FILTER_LINEAR;
  info.minFilter = VK_FILTER_LINEAR;
  info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.anisotropyEnable = VK_TRUE;
  info.maxAnisotropy = 16;
  info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info.unnormalizedCoordinates = VK_FALSE;
  info.compareEnable = VK_FALSE;
  info.compareOp = VK_COMPARE_OP_ALWAYS;
  info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  info.minLod = 11;
  info.maxLod = 0;

  auto destroySampler = [device = vkDevice_](auto sampler) {
    vkDestroySampler(device, sampler, nullptr);
    LOG(trace) << "Texture Sampler destroyed";
  };
  VkSampler sampler;
  if (vkCreateSampler(vkDevice_, &info, nullptr, &sampler) != VK_SUCCESS) {
    throw std::runtime_error("Could not create Texture Sampler");
  }
  textureSampler_.set(sampler, destroySampler);
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::createRenderTarget()
{
  targetImage_ = std::make_unique<Image>(device_, allocator_, size_, format_, msaa_,
    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  targetImageView_ = std::make_unique<ImageView>(device_, targetImage_.get(), VK_IMAGE_ASPECT_COLOR_BIT);
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::createDepthImage()
{
  depthFormat_ = getDepthImageFormat(
    device_->physical(),
    { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
    VK_IMAGE_TILING_OPTIMAL,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
  );
  depthImage_ = std::make_unique<Image>(device_, allocator_, size_, depthFormat_, msaa_,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  depthImageView_ = std::make_unique<ImageView>(device_, depthImage_.get(), VK_IMAGE_ASPECT_DEPTH_BIT);
}

// -------------------------------------------------------------------------------------------------------------------------
void Swapchain::tmpUpdate(uint32_t index)
{
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  UniformObject uniform = {};
  uniform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  uniform.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  uniform.projection = glm::perspective(glm::radians(45.0f), size_.width / (float)size_.height, 0.1f, 10.0f);
  uniform.projection[1][1] *= -1;
  uniformBuffers_[index]->update(&uniform, uniBufferSize);
}

} // !namespace yaga
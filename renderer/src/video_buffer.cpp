#include "precompiled.h"
#include "video_buffer.h"
#include "device.h"
#include "image_view.h"
#include "asset/vertex.h"

namespace yaga
{
namespace
{

constexpr auto uniBufferSize = sizeof(UniformObject);

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
VideoBuffer::VideoBuffer(Device* device, Allocator* allocator, VkSurfaceKHR surface, VkExtent2D size)
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
  info.imageExtent = PickResolution(capabilities, size);
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  format_ = info.imageFormat;
  size_ = info.imageExtent;

  const auto& queueFamilies = device->QueueFamilies();
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
  info.presentMode = PickBufferingMode(device->Physical(), surface);
  info.clipped = VK_TRUE;

  auto logicalDeivce = device->Logical();
  auto destroySwapchain = [logicalDeivce](auto chain) {
    vkDestroySwapchainKHR(logicalDeivce, chain, nullptr);
    LOG(trace) << "Swapchain destroyed";
  };
  VkSwapchainKHR chain;
  if (vkCreateSwapchainKHR(logicalDeivce, &info, nullptr, &chain) != VK_SUCCESS) {
    THROW("Could not create Swapchain");
  }
  swapchain_.Assign(chain, destroySwapchain);
  LOG(trace) << "Swapchain created";

  uint32_t imageCount;
  vkGetSwapchainImagesKHR(logicalDeivce, *swapchain_, &imageCount, nullptr);
  images_.resize(imageCount);
  imageViews_.resize(imageCount);
  vkGetSwapchainImagesKHR(logicalDeivce, *swapchain_, &imageCount, images_.data());
  for (uint32_t i = 0; i < imageCount; ++i) {
    imageViews_[i] = std::make_unique<ImageView>(logicalDeivce, images_[i], colorFormat.format);
  }

  CreateUniformBuffers(logicalDeivce, allocator);
  CreateTextureSampler(logicalDeivce);
  CreateDescriptorPool(logicalDeivce);
  CreateDescriptorLayout(logicalDeivce);
  //CreateDescriptorSets(logicalDeivce);
  CreatePipelineLayout(logicalDeivce); 
}

// -------------------------------------------------------------------------------------------------------------------------
VideoBuffer::~VideoBuffer()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void VideoBuffer::CreateDescriptorPool(VkDevice device)
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
  auto destroyPool = [this, device](auto pool) {
    vkDestroyDescriptorPool(device, pool, nullptr);
    LOG(trace) << "Descriptor Pool destroyed";
  };
  if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
    throw std::runtime_error("Could not create descriptor pool");
  }
  descriptorPool_.Assign(pool, destroyPool);
}

// -------------------------------------------------------------------------------------------------------------------------
void VideoBuffer::CreatePipelineLayout(VkDevice device)
{
  VkPipelineLayoutCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = 1;
  info.pushConstantRangeCount = 0;
  info.pSetLayouts = &*descriptorSetLayout_;

  auto destroyLayout = [device](auto layout) {
    vkDestroyPipelineLayout(device, layout, nullptr);
    LOG(trace) << "Pipeline Layout destroyed";
  };
  VkPipelineLayout layout;
  if (vkCreatePipelineLayout(device, &info, nullptr, &layout) != VK_SUCCESS) {
    THROW("Could not create Pipeline Layout");
  }
  pipelineLayout_.Assign(layout, destroyLayout);
  LOG(trace) << "Pipeline Layout created";
}

// -------------------------------------------------------------------------------------------------------------------------
void VideoBuffer::CreateDescriptorLayout(VkDevice device)
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

  auto destroyLayout = [device](auto layout) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    LOG(trace) << "Descriptor Set Layout destroyed";
  };
  VkDescriptorSetLayout layout;
  if (vkCreateDescriptorSetLayout(device, &info, nullptr, &layout) != VK_SUCCESS) {
    throw std::runtime_error("Could not create Descriptor Set Layout!");
  }
  descriptorSetLayout_.Assign(layout, destroyLayout);
}

// -------------------------------------------------------------------------------------------------------------------------
void VideoBuffer::CreateUniformBuffers(VkDevice device, Allocator* allocator)
{
  uniformBuffers_.resize(images_.size());
  for (size_t i = 0; i < images_.size(); i++) {
    uniformBuffers_[i] = std::make_unique<DeviceBuffer>(device, allocator, uniBufferSize,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void VideoBuffer::CreateTextureSampler(VkDevice device)
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

  auto destroySampler = [device](auto sampler) {
    vkDestroySampler(device, sampler, nullptr);
    LOG(trace) << "Texture Sampler destroyed";
  };
  VkSampler sampler;
  if (vkCreateSampler(device, &info, nullptr, &sampler) != VK_SUCCESS) {
    throw std::runtime_error("Could not create Texture Sampler");
  }
  textureSampler_.Assign(sampler, destroySampler);
}

// -------------------------------------------------------------------------------------------------------------------------
/*void VideoBuffer::CreateDescriptorSets(VkDevice device)
{
  std::vector<VkDescriptorSetLayout> layouts(images_.size(), *descriptorSetLayout_);
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = *descriptorPool_;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets_.resize(images_.size());
  if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets_.data()) != VK_SUCCESS) {
    throw std::runtime_error("Could not allocate descriptor sets");
  }

  for (size_t i = 0; i < images_.size(); i++) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = **uniformBuffers_[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformObject);

    std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets_[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = **imageViews_[i];
    imageInfo.sampler = *textureSampler_;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets_[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
  }
}*/

// -------------------------------------------------------------------------------------------------------------------------
void VideoBuffer::TmpUpdate(uint32_t index)
{
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  UniformObject uniform = {};
  uniform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  uniform.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  uniform.projection = glm::perspective(glm::radians(45.0f), size_.width / (float)size_.height, 0.1f, 10.0f);
  uniform.projection[1][1] *= -1;
  uniformBuffers_[index]->Update(&uniform, uniBufferSize);
}

} // !namespace yaga
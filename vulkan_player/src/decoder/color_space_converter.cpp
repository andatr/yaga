#include "precompiled.h"
#include "color_space_converter.h"
#include "vulkan_renderer/vulkan_utils.h"

#define IMAGES_SIZE static_cast<uint32_t>(images_.size())

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
void changeLayoutToGeneral(VkCommandBuffer command, VkImage image, uint32_t srcFamily, uint32_t dstFamily)
{
  VkImageMemoryBarrier barrier{};
  barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image                           = image;
  barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout                       = VK_IMAGE_LAYOUT_GENERAL;
  barrier.srcAccessMask                   = VK_ACCESS_NONE_KHR;
  barrier.dstAccessMask                   = VK_ACCESS_SHADER_WRITE_BIT;
  barrier.srcQueueFamilyIndex             = srcFamily;
  barrier.dstQueueFamilyIndex             = dstFamily;
  barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel   = 0;
  barrier.subresourceRange.levelCount     = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount     = 1;
  vkCmdPipelineBarrier(
    command,
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
  );
}

// -----------------------------------------------------------------------------------------------------------------------------
void changeLayoutFromGeneral(VkCommandBuffer command, VkImage image, VkImageLayout layout, uint32_t srcFamily, uint32_t dstFamily)
{
  VkImageMemoryBarrier barrier{};
  barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image                           = image;
  barrier.oldLayout                       = VK_IMAGE_LAYOUT_GENERAL;
  barrier.newLayout                       = layout;
  barrier.srcAccessMask                   = VK_ACCESS_SHADER_WRITE_BIT;
  barrier.dstAccessMask                   = VK_ACCESS_NONE_KHR;
  barrier.srcQueueFamilyIndex             = srcFamily;
  barrier.dstQueueFamilyIndex             = dstFamily;
  barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel   = 0;
  barrier.subresourceRange.levelCount     = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount     = 1;
  vkCmdPipelineBarrier(
    command,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
  );
}

} // !namespace

extern const unsigned char colorSpaceCompShader[];
extern const size_t colorSpaceCompShaderSize;

// -----------------------------------------------------------------------------------------------------------------------------
ColorSpaceConverter::ColorSpaceConverter(Swapchain* swapchain, VmaAllocator allocator, size_t images, VkImageLayout destLayout) :
  swapchain_(swapchain),
  allocator_(allocator),
  images_(images),
  destLayout_(destLayout)
{
  createDescriptorLayout();
  createPipelineLayout();
  createShaders();
  createPipeline();
  createDescriptorPool();
  createDescriptors();
  createCommandPool();
  createCommand();
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::waitIdle()
{
  const auto device = **swapchain_->device();
  vkWaitForFences(device, 1, &*fence_, VK_TRUE, UINT64_MAX);
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::update(const std::vector<Images>& images, FormatPtr format)
{
  waitIdle();
  images_ = images;
  format_ = format;
  updateDescriptors();
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::convert(int index)
{
  const auto device = **swapchain_->device();
  const auto compf  = swapchain_->device()->queueFamilies().compute;
  const auto graphf = swapchain_->device()->queueFamilies().graphics;
  const auto queue  = swapchain_->device()->computeQueue();
  const auto width  = std::min(swapchain_->resolution().width, static_cast<uint32_t>(format_->luma.width));
  const auto height = std::min(swapchain_->resolution().height, static_cast<uint32_t>(format_->luma.height));
  const auto dest   = images_[index].dest.data;

  vkWaitForFences(device, 1, &*fence_, VK_TRUE, UINT64_MAX);
  vkResetFences  (device, 1, &*fence_);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VULKAN_GUARD(vkBeginCommandBuffer(*command_, &beginInfo), "Could not begin Compute Command");
  changeLayoutToGeneral            (*command_, dest, graphf, compf);
  vkCmdBindPipeline                (*command_, VK_PIPELINE_BIND_POINT_COMPUTE, *pipeline_);
  vkCmdBindDescriptorSets          (*command_, VK_PIPELINE_BIND_POINT_COMPUTE, *pipelineLayout_, 0, 1, &descriptors_[index], 0, 0);
  vkCmdDispatch                    (*command_, width, height, 1);
  changeLayoutFromGeneral          (*command_, dest, destLayout_, compf, graphf);
  VULKAN_GUARD(vkEndCommandBuffer  (*command_), "Could not finish Compute Command");

  VkSubmitInfo submitInfo{};
  submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &*command_;

  VULKAN_GUARD(vkQueueSubmit(queue, 1, &submitInfo, *fence_), "Could not submit Compute Command");
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::createDescriptorLayout()
{
  constexpr uint32_t size = 3;
  std::array<VkDescriptorSetLayoutBinding, size> computeBinding{};
  for (uint32_t i = 0; i < size; ++i) {
    computeBinding[i].binding         = i;
    computeBinding[i].descriptorCount = 1;
    computeBinding[i].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    computeBinding[i].stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT;
  }
  VkDescriptorSetLayoutCreateInfo info{};
  info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = size;
  info.pBindings    = computeBinding.data();
  descriptorLayout_ = vk::createLayout(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::createPipelineLayout()
{
  VkPipelineLayoutCreateInfo info{};
  info.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = 1;
  info.pSetLayouts    = &*descriptorLayout_;
  pipelineLayout_ = vk::createLayout(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::createShaders()
{
  VkShaderModuleCreateInfo info{};
  info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.codeSize = colorSpaceCompShaderSize;
  info.pCode    = reinterpret_cast<const uint32_t*>(colorSpaceCompShader);
  shader_  = createShader(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::createPipeline()
{
  VkComputePipelineCreateInfo info{};
  info.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  info.layout = *pipelineLayout_;
  info.stage  = getShaderStageInfo(*shader_, VK_SHADER_STAGE_COMPUTE_BIT, "main");
  pipeline_ = vk::createPipeline(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::createDescriptorPool()
{
  VkDescriptorPoolSize size{};
  size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  size.descriptorCount = IMAGES_SIZE * 3;

  VkDescriptorPoolCreateInfo info{};
  info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  info.poolSizeCount = 1;
  info.pPoolSizes    = &size;
  info.maxSets       = IMAGES_SIZE;
  descriptorPool_ = vk::createDescriptorPool(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::createDescriptors()
{
  std::vector<VkDescriptorSetLayout> layouts(IMAGES_SIZE, *descriptorLayout_);
  const auto device = **swapchain_->device();
  VkDescriptorSetAllocateInfo info{};
  info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  info.descriptorPool     = *descriptorPool_;
  info.descriptorSetCount = IMAGES_SIZE;
  info.pSetLayouts        = layouts.data();
  descriptors_ = vk::createDescriptors(device, info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::updateDescriptors()
{
  const uint32_t size = IMAGES_SIZE * 3;
  std::vector<VkImageView>            views(size);
  std::vector<VkWriteDescriptorSet> writers(size);
  std::vector<VkDescriptorImageInfo>  infos(size);
  for (size_t i = 0; i < images_.size(); ++i) {
    views[i * 3 + 0] = images_[i].luma.view;
    views[i * 3 + 1] = images_[i].chroma.view;
    views[i * 3 + 2] = images_[i].dest.view;
  }
  for (uint32_t i = 0; i < size; ++i) {
    infos[i].imageLayout       = VK_IMAGE_LAYOUT_GENERAL;
    infos[i].imageView         = views[i];
    writers[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writers[i].dstSet          = descriptors_[i / 3];
    writers[i].dstBinding      = i % 3;
    writers[i].dstArrayElement = 0;
    writers[i].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writers[i].descriptorCount = 1;
    writers[i].pImageInfo      = &infos[i];
  }
  vk::updateDescriptor(**swapchain_->device(), writers.data(), writers.size());
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::createCommandPool()
{
  VkCommandPoolCreateInfo info{};
  info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = swapchain_->device()->queueFamilies().compute;
  commandPool_ = vk::createCommandPool(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::createCommand()
{
  VkCommandBufferAllocateInfo info{};
  info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  info.commandPool        = *commandPool_;
  info.commandBufferCount = 1;
  command_ = vk::createCommand(**swapchain_->device(), *commandPool_, info);
  fence_ = createFence(**swapchain_->device());
}

} // !namespace vk
} // !namespace yaga
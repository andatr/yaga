#include "precompiled.h"
#include "vulkan_renderer/render_layout_3d.h"
#include "vulkan_renderer/uniform.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
RenderLayout3D::RenderLayout3D(VkDevice device)
{
  createUniform (device);
  createTexture (device);
  createPipeline(device);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderLayout3D::createUniform(VkDevice device)
{
  VkDescriptorSetLayoutBinding uboBinding{};
  uboBinding.binding            = 0;
  uboBinding.descriptorCount    = 1;
  uboBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboBinding.pImmutableSamplers = nullptr;
  uboBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = 1;
  info.pBindings    = &uboBinding;

  auto destroyLayout = [device](auto layout) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    LOG(trace) << "Descriptor Set Layout destroyed";
  };

  VkDescriptorSetLayout layout{};
  VULKAN_GUARD(vkCreateDescriptorSetLayout(device, &info, nullptr, &layout),
    "Could not create Uniform Descriptor Layout");
  uniform_.set(layout, destroyLayout);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderLayout3D::createTexture(VkDevice device)
{
  VkDescriptorSetLayoutBinding samplerBinding{};
  samplerBinding.binding            = 0;
  samplerBinding.descriptorCount    = 1;
  samplerBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerBinding.pImmutableSamplers = nullptr;
  samplerBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = 1;
  info.pBindings    = &samplerBinding;

  auto destroyLayout = [device](auto layout) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    LOG(trace) << "Descriptor Set Layout destroyed";
  };

  VkDescriptorSetLayout layout{};
  VULKAN_GUARD(vkCreateDescriptorSetLayout(device, &info, nullptr, &layout),
    "Could not create material Descriptor Set Layout");
  texture_.set(layout, destroyLayout);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderLayout3D::createPipeline(VkDevice device)
{
  VkPushConstantRange pushConstant {};
  pushConstant.offset     = 0;
  pushConstant.size       = sizeof(PushConstantVertex);
  pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayout setLayouts[2] { *uniform_, *texture_ };
  VkPipelineLayoutCreateInfo info{};
  info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.pushConstantRangeCount = 1;
  info.pPushConstantRanges    = &pushConstant;
  info.setLayoutCount         = 2;
  info.pSetLayouts            = setLayouts;

  pipeline_ = vk::createLayout(device, info);
}

} // !namespace vk
} // !namespace yaga

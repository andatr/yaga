#include "precompiled.h"
#include "material_pool.h"
#include "shader.h"
#include "uniform.h"
#include "assets/vertex.h"

namespace yaga
{
namespace vk
{
namespace
{

// -------------------------------------------------------------------------------------------------------------------------
VkVertexInputBindingDescription getVertexBindingDescription()
{
  VkVertexInputBindingDescription bindingDescription {};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescription;
}

// -------------------------------------------------------------------------------------------------------------------------
std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescriptions()
{
  std::array<VkVertexInputAttributeDescription, 3> desc {};
  desc[0].binding = 0;
  desc[0].location = 0;
  desc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  desc[0].offset = offsetof(Vertex, pos);
  desc[1].binding = 0;
  desc[1].location = 1;
  desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  desc[1].offset = offsetof(Vertex, color);
  desc[2].binding = 0;
  desc[2].location = 2;
  desc[2].format = VK_FORMAT_R32G32_SFLOAT;
  desc[2].offset = offsetof(Vertex, uv);
  return desc;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineInputAssemblyStateCreateInfo getPrimitives()
{
  VkPipelineInputAssemblyStateCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  info.primitiveRestartEnable = VK_FALSE;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkViewport getViewport(const VkExtent2D& size)
{
  VkViewport viewport {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)size.width;
  viewport.height = (float)size.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  return viewport;
}

// -------------------------------------------------------------------------------------------------------------------------
VkRect2D getScissors(const VkExtent2D& size)
{
  VkRect2D scissors {};
  scissors.offset = { 0, 0 };
  scissors.extent = size;
  return scissors;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineViewportStateCreateInfo getViewportState(const VkViewport& viewport, const VkRect2D& scissors)
{
  VkPipelineViewportStateCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  info.viewportCount = 1;
  info.pViewports = &viewport;
  info.scissorCount = 1;
  info.pScissors = &scissors;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineRasterizationStateCreateInfo getRasterizer()
{
  VkPipelineRasterizationStateCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  info.depthClampEnable = VK_FALSE;
  info.rasterizerDiscardEnable = VK_FALSE;
  info.polygonMode = VK_POLYGON_MODE_FILL;
  info.lineWidth = 1.0f;
  info.cullMode = VK_CULL_MODE_BACK_BIT;
  info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  //info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  info.depthBiasEnable = VK_FALSE;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineMultisampleStateCreateInfo getSampler(VkSampleCountFlagBits msaa)
{
  VkPipelineMultisampleStateCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  info.sampleShadingEnable = VK_FALSE;
  info.minSampleShading = 1.0f;
  info.rasterizationSamples = msaa;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineColorBlendAttachmentState getColorBlender()
{
  VkPipelineColorBlendAttachmentState info {};
  info.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  info.blendEnable = VK_FALSE;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineColorBlendStateCreateInfo getColorBlendState(const VkPipelineColorBlendAttachmentState& blender)
{
  VkPipelineColorBlendStateCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  info.logicOpEnable = VK_FALSE;
  info.logicOp = VK_LOGIC_OP_COPY;
  info.attachmentCount = 1;
  info.pAttachments = &blender;
  info.blendConstants[0] = 0.0f;
  info.blendConstants[1] = 0.0f;
  info.blendConstants[2] = 0.0f;
  info.blendConstants[3] = 0.0f;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineDepthStencilStateCreateInfo getDepthStencilState()
{
  VkPipelineDepthStencilStateCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  info.depthTestEnable = VK_TRUE;
  info.depthWriteEnable = VK_TRUE;
  info.depthCompareOp = VK_COMPARE_OP_LESS;
  info.depthBoundsTestEnable = VK_FALSE;
  info.stencilTestEnable = VK_FALSE;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineShaderStageCreateInfo getShaderStage(VkShaderModule module, VkShaderStageFlagBits shaderType)
{
  VkPipelineShaderStageCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = shaderType;
  info.module = module;
  info.pName = "main";
  return info;
}

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
MaterialPool::MaterialPool(Device* device, Swapchain* swapchain, ImagePool* imagePool,
  VkDescriptorPool descriptorPool, VkDescriptorSetLayout uniformLayout) :
    vkDevice_(**device), swapchain_(swapchain), imagePool_(imagePool), descriptorPool_(descriptorPool), 
    uniformLayout_(uniformLayout), frames_(static_cast<uint32_t>(swapchain->frameBuffers().size()))
{
  createDescriptorLayout();
  createPipelineLayout();
}

// -------------------------------------------------------------------------------------------------------------------------
void MaterialPool::swapchain(Swapchain* swapchain)
{
  swapchain_ = swapchain;
  for (const auto& material : materials_) {
    const auto& asset = material->asset();
    auto pipeline = createPipeline(asset->vertexShader(), asset->fragmentShader());
    material->pipeline_ = *pipeline;
    materialCache_[asset].pipeline = std::move(pipeline);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void MaterialPool::createDescriptorLayout()
{
  VkDescriptorSetLayoutBinding samplerBinding {};
  samplerBinding.binding = 0;
  samplerBinding.descriptorCount = 1;
  samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerBinding.pImmutableSamplers = nullptr;
  samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  auto destroyLayout = [device = vkDevice_](auto layout) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    LOG(trace) << "Descriptor Set Layout destroyed";
  };

  VkDescriptorSetLayoutCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = 1;
  info.pBindings = &samplerBinding;

  VkDescriptorSetLayout layout;
  VULKAN_GUARD(vkCreateDescriptorSetLayout(vkDevice_, &info, nullptr, &layout),
    "Could not create material Descriptor Set Layout");
  descriptorLayout_.set(layout, destroyLayout);
}

// -------------------------------------------------------------------------------------------------------------------------
void MaterialPool::createPipelineLayout()
{
  std::array<VkPushConstantRange, 1> pushConstants;
  pushConstants[0].offset = 0;
  pushConstants[0].size = sizeof(PushConstantVertex);
  pushConstants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  std::array<VkDescriptorSetLayout, 2> setLayouts = { uniformLayout_, *descriptorLayout_ };
  VkPipelineLayoutCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
  info.pPushConstantRanges = pushConstants.data();
  info.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
  info.pSetLayouts = setLayouts.data();

  auto destroyLayout = [device = vkDevice_](auto layout) {
    vkDestroyPipelineLayout(device, layout, nullptr);
    LOG(trace) << "Pipeline Layout destroyed";
  };
  VkPipelineLayout layout;
  VULKAN_GUARD(vkCreatePipelineLayout(vkDevice_, &info, nullptr, &layout), "Could not create Pipeline Layout");
  pipelineLayout_.set(layout, destroyLayout);
  LOG(trace) << "Pipeline Layout created";
}

// ------------------------------------------------------------------------------------------------------------------------
void MaterialPool::clear()
{
  if (!materials_.empty()) {
    THROW("Can not clear Material Pool while its components are still in use");
  }
  materialCache_.clear();
  shaderCache_.clear();
}

// ------------------------------------------------------------------------------------------------------------------------
void MaterialPool::removeMaterial(Material* material)
{
  vkDeviceWaitIdle(vkDevice_);
  materials_.erase(material);
}

// ------------------------------------------------------------------------------------------------------------------------
MaterialPtr MaterialPool::createMaterial(Object* object, assets::Material* asset)
{
  auto it = materialCache_.find(asset);
  if (it != materialCache_.end()) {
    auto material = std::make_unique<Material>(object, asset, this, *it->second.pipeline,
      *pipelineLayout_, it->second.descriptorSets);
    materials_.insert(material.get());
    return material;
  }

  MaterialCache cache;
  cache.descriptorSets = createDescriptorSets();
  cache.pipeline = createPipeline(asset->vertexShader(), asset->fragmentShader());
  auto material = std::make_unique<Material>(object, asset, this, *cache.pipeline, *pipelineLayout_, cache.descriptorSets);
  materials_.insert(material.get());
  materialCache_[asset] = std::move(cache);

  std::vector<Image*> images;
  images.reserve(asset->textures().size());
  for (const auto& textureAsset : asset->textures()) {
    images.push_back(imagePool_->createImage(textureAsset));
  }
  updateDescriptorSets(material->descriptorSets_, images);
  return material;
}

// -------------------------------------------------------------------------------------------------------------------------
VkShaderModule MaterialPool::createShader(assets::Shader* asset)
{
  auto it = shaderCache_.find(asset);
  if (it != shaderCache_.end()) return *it->second;

  VkShaderModuleCreateInfo createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = asset->code().size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(asset->code().data());

  auto destroyShader = [device = vkDevice_](auto shader) {
    vkDestroyShaderModule(device, shader, nullptr);
    LOG(trace) << "Shader destroyed";
  };

  VkShaderModule shader;
  VULKAN_GUARD(vkCreateShaderModule(vkDevice_, &createInfo, nullptr, &shader), "Could not create Shader");
  shaderCache_[asset] = AutoDestructor<VkShaderModule>(shader, destroyShader);
  LOG(trace) << "Shader created";
  return shader;
}

// -------------------------------------------------------------------------------------------------------------------------
AutoDestructor<VkPipeline> MaterialPool::createPipeline(assets::Shader* vertexShader, assets::Shader* fragmentShader)
{
  VkPipelineShaderStageCreateInfo shaderStages[] = {
    getShaderStage(createShader(vertexShader),  VK_SHADER_STAGE_VERTEX_BIT),
    getShaderStage(createShader(fragmentShader), VK_SHADER_STAGE_FRAGMENT_BIT)
  };

  auto bindingDescription = getVertexBindingDescription();
  auto attributeDescriptions = getVertexAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexAttributes {};
  vertexAttributes.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexAttributes.vertexBindingDescriptionCount = 1;
  vertexAttributes.pVertexBindingDescriptions = &bindingDescription;
  vertexAttributes.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexAttributes.pVertexAttributeDescriptions = attributeDescriptions.data();

  auto primitives = getPrimitives();
  auto viewport = getViewport(swapchain_->resolution());
  auto scissors = getScissors(swapchain_->resolution());
  auto viewportState = getViewportState(viewport, scissors);
  auto rasterizer = getRasterizer();
  auto sampler = getSampler(swapchain_->msaa());
  auto colorBlender = getColorBlender();
  auto colorBlendState = getColorBlendState(colorBlender);
  auto depthStencilState = getDepthStencilState();
  
  VkGraphicsPipelineCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info.pVertexInputState = &vertexAttributes;
  info.pInputAssemblyState = &primitives;
  info.pViewportState = &viewportState;
  info.pRasterizationState = &rasterizer;
  info.pMultisampleState = &sampler;
  info.pColorBlendState = &colorBlendState;
  info.pDepthStencilState = &depthStencilState;
  info.layout = *pipelineLayout_;
  info.renderPass = swapchain_->renderPass();
  info.subpass = 0;
  info.basePipelineHandle = VK_NULL_HANDLE;
  info.stageCount = 2;
  info.pStages = shaderStages;

  auto destroyPipeline = [device = vkDevice_](auto pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
    LOG(trace) << "Pipeline destroyed";
  };

  VkPipeline pipeline;
  VULKAN_GUARD(vkCreateGraphicsPipelines(vkDevice_, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline),
    "Could not create Pipeline");
  return AutoDestructor<VkPipeline>(pipeline, destroyPipeline);
}

// -------------------------------------------------------------------------------------------------------------------------
std::vector<VkDescriptorSet> MaterialPool::createDescriptorSets() const
{
  std::vector<VkDescriptorSetLayout> layouts(frames_, *descriptorLayout_);
  VkDescriptorSetAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool_;
  allocInfo.descriptorSetCount = frames_;
  allocInfo.pSetLayouts = layouts.data();

  std::vector<VkDescriptorSet> descriptorSets(frames_);
  VULKAN_GUARD(vkAllocateDescriptorSets(vkDevice_, &allocInfo, descriptorSets.data()), "Could not allocate Descriptor Sets");
  return descriptorSets;
}

// -------------------------------------------------------------------------------------------------------------------------
void MaterialPool::updateDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets,
  const std::vector<Image*>& images) const
{
  std::vector<VkWriteDescriptorSet> writers(images.size() * frames_);
  for (size_t i = 0; i < frames_; ++i) {
    VkDescriptorImageInfo imageInfo {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    for (size_t j = 0; j < images.size(); ++j) {
      auto index = i * images.size() + j;
      imageInfo.imageView = images[j]->view();
      imageInfo.sampler = images[j]->sampler();
      writers[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writers[index].dstSet = descriptorSets[i];
      writers[index].dstBinding = static_cast<uint32_t>(j);
      writers[index].dstArrayElement = 0;
      writers[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writers[index].descriptorCount = 1;
      writers[index].pImageInfo = &imageInfo;
    }
  }
  vkUpdateDescriptorSets(vkDevice_, static_cast<uint32_t>(writers.size()), writers.data(), 0, nullptr);
}

} // !namespace vk
} // !namespace yaga
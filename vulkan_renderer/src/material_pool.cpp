#include "precompiled.h"
#include "material_pool.h"
#include "assets/vertex.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
VkVertexInputBindingDescription getVertexBindingDescription()
{
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding   = 0;
  bindingDescription.stride    = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescription;
}

// -----------------------------------------------------------------------------------------------------------------------------
std::array<VkVertexInputAttributeDescription, 5> getVertexAttributeDescriptions()
{
  std::array<VkVertexInputAttributeDescription, 5> desc{};
  desc[0].binding  = 0;
  desc[0].location = 0;
  desc[0].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
  desc[0].offset   = offsetof(Vertex, position);

  desc[1].binding  = 0;
  desc[1].location = 1;
  desc[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
  desc[1].offset   = offsetof(Vertex, normal);

  desc[2].binding  = 0;
  desc[2].location = 2;
  desc[2].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
  desc[2].offset   = offsetof(Vertex, tangent);

  desc[3].binding  = 0;
  desc[3].location = 3;
  desc[3].format   = VK_FORMAT_R32G32_SFLOAT;
  desc[3].offset   = offsetof(Vertex, texture);

  desc[4].binding  = 0;
  desc[4].location = 4;
  desc[4].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
  desc[4].offset   = offsetof(Vertex, color);

  /*desc[4].binding  = 0;
  desc[4].location = 4;
  desc[4].format   = VK_FORMAT_R32G32_SFLOAT;
  desc[4].offset   = offsetof(Vertex, texCoord);*/
  return desc;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineInputAssemblyStateCreateInfo getPrimitives()
{
  VkPipelineInputAssemblyStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  info.primitiveRestartEnable = VK_FALSE;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkViewport getViewport(const VkExtent2D& size)
{
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width  = (float)size.width;
  viewport.height = (float)size.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  return viewport;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkRect2D getScissors(const VkExtent2D& size)
{
  VkRect2D scissors{};
  scissors.offset = { 0, 0 };
  scissors.extent = size;
  return scissors;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineViewportStateCreateInfo getViewportState(const VkViewport& viewport, const VkRect2D& scissors)
{
  VkPipelineViewportStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  info.viewportCount = 1;
  info.pViewports    = &viewport;
  info.scissorCount  = 1;
  info.pScissors     = &scissors;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineRasterizationStateCreateInfo getRasterizer(bool wireframe)
{
  VkPipelineRasterizationStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  info.rasterizerDiscardEnable = VK_FALSE;
  info.depthClampEnable = VK_FALSE;
  info.depthBiasEnable  = VK_FALSE;
  info.polygonMode = wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
  info.lineWidth   = 1.0f;
  info.cullMode    = VK_CULL_MODE_NONE; // VK_CULL_MODE_BACK_BIT;
  info.frontFace   = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineMultisampleStateCreateInfo getSampler(VkSampleCountFlagBits msaa)
{
  VkPipelineMultisampleStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  info.sampleShadingEnable  = VK_FALSE;
  info.minSampleShading     = 1.0f;
  info.rasterizationSamples = msaa;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineColorBlendAttachmentState getColorBlender()
{
  VkPipelineColorBlendAttachmentState info{};
  info.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | 
    VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT | 
    VK_COLOR_COMPONENT_A_BIT;
  info.blendEnable = VK_FALSE;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineColorBlendStateCreateInfo getColorBlendState(const VkPipelineColorBlendAttachmentState& blender)
{
  VkPipelineColorBlendStateCreateInfo info{};
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

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineDepthStencilStateCreateInfo getDepthStencilState()
{
  VkPipelineDepthStencilStateCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  info.depthTestEnable  = VK_TRUE;
  info.depthWriteEnable = VK_TRUE;
  info.depthCompareOp = VK_COMPARE_OP_LESS;
  info.depthBoundsTestEnable = VK_FALSE;
  info.stencilTestEnable = VK_FALSE;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineShaderStageCreateInfo getShaderStage(VkShaderModule module, VkShaderStageFlagBits shaderType)
{
  VkPipelineShaderStageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = shaderType;
  info.module = module;
  info.pName = "main";
  return info;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPool::MaterialPool(Swapchain* swapchain,
  VmaAllocator allocator,
  RenderPass* renderPass,
  const Config& config) :
    counter_(0),
    swapchain_(swapchain),
    renderPass_(renderPass)
{
  shaderPool_  = std::make_unique<ShaderPool> (swapchain->device());
  texturePool_ = std::make_unique<TexturePool>(swapchain->device(), allocator, config.maxImageSize());
}

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPool::~MaterialPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPtr MaterialPool::get(Object* object, assets::Material* asset)
{
  ++counter_;
  auto it = materials_.find(asset);
  if (it != materials_.end()) {
    return std::make_unique<Material>(this, object, asset, it->second.get());
  }

  auto pipeline = createPipeline(asset);
  auto pipelinePtr = pipeline.get();
  materials_[asset] = std::move(pipeline);

  std::vector<Image*> textures;
  textures.reserve(asset->textures().size());
  for (const auto& textureAsset : asset->textures()) {
    textures.push_back(texturePool_->get(textureAsset));
  }
  createDescriptors(pipelinePtr);
  updateDescriptors(pipelinePtr, textures);

  return std::make_unique<Material>(this, object, asset, pipelinePtr);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MaterialPool::onRemove(Material*)
{
  if (counter_ > 0) {
    --counter_;
  }
  else {
    THROW("MaterialPool::onRemove something went wrong");
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void MaterialPool::clear()
{
  if (counter_ != 0) {
    THROW("Not all materials were returned to the pool");
  }
  materials_.clear();
  texturePool_->clear();
  shaderPool_->clear();
}

// -----------------------------------------------------------------------------------------------------------------------------
PipelinePtr MaterialPool::createPipeline(assets::Material* asset) const
{
  std::array<VkPipelineShaderStageCreateInfo, 2> shaders = {
    getShaderStage(shaderPool_->get(asset->vertexShader()),   VK_SHADER_STAGE_VERTEX_BIT),
    getShaderStage(shaderPool_->get(asset->fragmentShader()), VK_SHADER_STAGE_FRAGMENT_BIT)
  };

  const auto bindingDescription    = getVertexBindingDescription();
  const auto attributeDescriptions = getVertexAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexAttributes{};
  vertexAttributes.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexAttributes.vertexBindingDescriptionCount   = 1;
  vertexAttributes.pVertexBindingDescriptions      = &bindingDescription;
  vertexAttributes.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexAttributes.pVertexAttributeDescriptions    = attributeDescriptions.data();

  std::array<VkDynamicState, 2> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };
  VkPipelineDynamicStateCreateInfo dynamicStatesInfo{};
  dynamicStatesInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStatesInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicStatesInfo.pDynamicStates    = dynamicStates.data();

  const auto primitives        = getPrimitives();
  const auto viewport          = getViewport(swapchain_->resolution());
  const auto scissors          = getScissors(swapchain_->resolution());
  const auto viewportState     = getViewportState(viewport, scissors);
        auto rasterizer        = getRasterizer(false);
  const auto sampler           = getSampler(swapchain_->msaa());
  const auto colorBlender      = getColorBlender();
  const auto colorBlendState   = getColorBlendState(colorBlender);
  const auto depthStencilState = getDepthStencilState();

  VkGraphicsPipelineCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info.pVertexInputState   = &vertexAttributes;
  info.pInputAssemblyState = &primitives;
  info.pViewportState      = &viewportState;
  info.pRasterizationState = &rasterizer;
  info.pMultisampleState   = &sampler;
  info.pColorBlendState    = &colorBlendState;
  info.pDepthStencilState  = &depthStencilState;
  info.pDynamicState       = &dynamicStatesInfo;
  info.layout              = renderPass_->pipelineLayout(),
  info.renderPass          = **renderPass_;
  info.subpass             = 0;
  info.basePipelineHandle  = VK_NULL_HANDLE;
  info.stageCount          = static_cast<uint32_t>(shaders.size());
  info.pStages             = shaders.data();

  auto destroyPipeline = [device = **swapchain_->device()](auto pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
    LOG(trace) << "Pipeline destroyed";
  };

  VkPipeline pipeline_, wireframe;
  VULKAN_GUARD(vkCreateGraphicsPipelines(**swapchain_->device(), VK_NULL_HANDLE, 1, &info, nullptr, &pipeline_),
    "Could not create Pipeline");
  rasterizer = getRasterizer(true);
  VULKAN_GUARD(vkCreateGraphicsPipelines(**swapchain_->device(), VK_NULL_HANDLE, 1, &info, nullptr, &wireframe),
    "Could not create wireframe Pipeline");

  auto pipeline = std::make_unique<Pipeline>();
  pipeline->main      = AutoDestructor<VkPipeline>(pipeline_, destroyPipeline);
  pipeline->wireframe = AutoDestructor<VkPipeline>(wireframe, destroyPipeline);
  return std::move(pipeline);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MaterialPool::createDescriptors(Pipeline* pipeline) const
{
  const auto frames = swapchain_->imageCount();
  std::vector<VkDescriptorSetLayout> layouts(frames, renderPass_->textureLayout());
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = swapchain_->device()->descriptorPool();
  allocInfo.descriptorSetCount = frames;
  allocInfo.pSetLayouts = layouts.data();

  pipeline->descriptors.resize(frames);
  VULKAN_GUARD(vkAllocateDescriptorSets(**swapchain_->device(), &allocInfo, pipeline->descriptors.data()),
    "Could not allocate Descriptor Sets");
}

// -----------------------------------------------------------------------------------------------------------------------------
void MaterialPool::updateDescriptors(Pipeline* pipeline, const std::vector<Image*>& textures) const
{
  const auto frames = swapchain_->imageCount();
  std::vector<VkWriteDescriptorSet> writers(textures.size() * frames);
  for (size_t i = 0; i < frames; ++i) {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    for (size_t j = 0; j < textures.size(); ++j) {
      auto index = i * textures.size() + j;
      imageInfo.sampler              = renderPass_->sampler();
      imageInfo.imageView            = textures[j]->view();
      writers[index].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writers[index].dstSet          = pipeline->descriptors[i];
      writers[index].dstBinding      = static_cast<uint32_t>(j);
      writers[index].dstArrayElement = 0;
      writers[index].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writers[index].descriptorCount = 1;
      writers[index].pImageInfo      = &imageInfo;
    }
  }
  vkUpdateDescriptorSets(**swapchain_->device(), static_cast<uint32_t>(writers.size()), writers.data(), 0, nullptr);
}

} // !namespace vk
} // !namespace yaga
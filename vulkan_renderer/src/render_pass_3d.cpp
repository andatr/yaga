#include "precompiled.h"
#include "vulkan_renderer/render_pass_3d.h"
#include "vulkan_renderer/uniform.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
VkViewport getViewport(const VkExtent2D& size)
{
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width  = static_cast<float>(size.width);
  viewport.height = static_cast<float>(size.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  return viewport;
}

// -----------------------------------------------------------------------------------------------------------------------------
std::vector<VkClearValue> getClearValues(const Config::Rendering& config)
{
  std::vector<VkClearValue> clearValues(2);
  clearValues[0].color = { 
    config.clearColor().x,
    config.clearColor().y,
    config.clearColor().z,
    config.clearColor().w
  };
  clearValues[1].depthStencil = { 
    config.clearDepth(),
    config.clearStencil()
  };
  return clearValues;
}

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
  info.rasterizationSamples = msaa > 0 ? msaa : VK_SAMPLE_COUNT_1_BIT;
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
RenderPass3D::RenderPass3D(Swapchain* swapchain, VmaAllocator allocator, ConfigPtr config) :
  RenderPass(swapchain),
  allocator_(allocator),
  layout_(**swapchain->device())
{
  clearValues_ = getClearValues(config->rendering());
  createDescriptorPool(config->resources());
  createUniform();
  createRenderPass();
  createFrameBuffers();
  cameraPool_   = std::make_unique<CameraPool>(); 
  materialPool_ = std::make_unique<MaterialPool>(allocator, this, config->resources());
  meshPool_     = std::make_unique<MeshPool>(swapchain->device(), allocator, config->resources());
  rendererPool_ = std::make_unique<Renderer3DPool>();
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::beginRender(uint32_t imageIndex)
{
  base::beginRender(imageIndex);
  const auto command  = frameBuffers_[imageIndex]->command();
  const auto viewport = getViewport(renderArea_.extent);
  vkCmdSetViewport(command, 0, 1, &viewport);
  vkCmdSetScissor (command, 0, 1, &renderArea_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::render(uint32_t imageIndex)
{
  updateUniform(imageIndex);
  for (const auto& object : rendererPool_->all()) {
    if (object->canRender()) {
      renderObject(imageIndex, object);
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::renderObject(uint32_t imageIndex, Renderer3D* object)
{
  const auto command  = frameBuffers_[imageIndex]->command();
  const auto mesh     = object->mesh();
  const auto material = object->material();
  mesh->update();
  VkBuffer vertexBuffers[] = { **mesh->vertexBuffer() };
  VkDeviceSize offsets[] = { 0 };
  const auto& pconst = object->pushConstant();
  const auto& pipeline = material->wireframe() ? *material->pipeline().wireframe : *material->pipeline().main;
  const auto& layout = layout_.pipeline();
  std::array<VkDescriptorSet, 2> descriptors{
    uniform_[imageIndex].descriptor,
    material->pipeline().descriptors[imageIndex]
  };
  const auto indexCount = static_cast<uint32_t>(mesh->asset()->indices().size());
  vkCmdBindPipeline      (command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 2, descriptors.data(), 0, nullptr);
  vkCmdPushConstants     (command, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantVertex), &pconst);
  vkCmdBindVertexBuffers (command, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer   (command, **mesh->indexBuffer(), 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed       (command, indexCount, 1, 0, 0, 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createUniform()
{
  const auto frames = swapchain_->imageCount();
  uniform_.resize(frames);

  std::vector<VkDescriptorSetLayout> layouts(frames, layout_.uniform());
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = *descriptorPool_;
  allocInfo.descriptorSetCount = frames;
  allocInfo.pSetLayouts = layouts.data();

  std::vector<VkDescriptorSet> sets(frames);
  VULKAN_GUARD(vkAllocateDescriptorSets(**swapchain_->device(), &allocInfo, sets.data()),
    "Could not allocate Uniform Descriptor Sets");
  
  VkBufferCreateInfo binfo{};
  binfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  binfo.size = sizeof(UniformObject);
  binfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  binfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo ballocInfo{};
  ballocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.offset = 0;
  bufferInfo.range = sizeof(UniformObject);

  VkWriteDescriptorSet writer{};
  writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writer.dstBinding = 0;
  writer.dstArrayElement = 0;
  writer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writer.descriptorCount = 1;
  writer.pBufferInfo = &bufferInfo;

  for (size_t i = 0; i < frames; ++i) {
    uniform_[i].descriptor = sets[i];
    uniform_[i].buffer = std::make_unique<Buffer>(allocator_, binfo, ballocInfo);
    bufferInfo.buffer = **uniform_[i].buffer;
    writer.dstSet = uniform_[i].descriptor;
    vkUpdateDescriptorSets(**swapchain_->device(), 1, &writer, 0, nullptr);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::updateUniform(uint32_t frame)
{
  auto camera = cameraPool_->mainCamera();
  if (!camera) return;
  const auto buffer = uniform_[frame].buffer.get();
  void* mappedData = nullptr;
  vmaMapMemory(allocator_, buffer->allocation(), &mappedData); 
  auto uniform = (UniformObject*)mappedData;
  uniform->view       = camera->view();
  uniform->projection = camera->projection();
  uniform->screen = { 
    static_cast<float>(swapchain_->resolution().width),
    static_cast<float>(swapchain_->resolution().height) 
  };
  uniform->aspect = uniform->screen.x / uniform->screen.y;
  memcpy(mappedData, mappedData, sizeof(UniformObject));
  vmaUnmapMemory(allocator_, buffer->allocation());
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createRenderPass()
{
  uint32_t attachmentIndex = 0;
  VkAttachmentDescription attachments[3];

  VkAttachmentDescription colorAttachment{};
  colorAttachment.format         = swapchain_->imageFormat();
  colorAttachment.samples        = swapchain_->msaaLevel();
  colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  attachments[attachmentIndex++] = colorAttachment;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment  = 0;
  colorAttachmentRef.layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  VkAttachmentReference   depthAttachmentRef{};
  if (swapchain_->depthFormat()) {
    depthAttachment.format         = swapchain_->depthFormat();
    depthAttachment.samples        = swapchain_->msaaLevel();
    depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[attachmentIndex++] = depthAttachment;

    depthAttachmentRef.attachment  = 1;
    depthAttachmentRef.layout      = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  }

  VkAttachmentDescription msaaAttachment{};
  VkAttachmentReference   msaaAttachmentRef{};
  if (swapchain_->msaaLevel()) {
    msaaAttachment.format          = swapchain_->imageFormat();
    msaaAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
    msaaAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    msaaAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
    msaaAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    msaaAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    msaaAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    msaaAttachment.finalLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[attachmentIndex++] = msaaAttachment;
    
    msaaAttachmentRef.attachment   = 2;
    msaaAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount    = 1;
  subpass.pColorAttachments       = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  subpass.pResolveAttachments     = &msaaAttachmentRef;

  // wait for the prev drawing to finish before we start a new one
  // allows us to use only one global depth and intermediate msaa buffer (otherwice would need one buffer per frame)
  // corresponds with finalStage_, you probably can remove this and set finalState_ to VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
  VkSubpassDependency dependency{};
  dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass    = 0;
  dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo info{};
  info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.attachmentCount = attachmentIndex;
  info.pAttachments    = attachments;
  info.subpassCount    = 1;
  info.pSubpasses      = &subpass;
  info.dependencyCount = 1;
  info.pDependencies   = &dependency;

  auto destroyRenderPass = [dev = **swapchain_->device()](auto renderPass) {
    vkDestroyRenderPass(dev, renderPass, nullptr);
    LOG(trace) << "3D Render Pass destroyed";
  };
  VkRenderPass renderPass{};
  VULKAN_GUARD(vkCreateRenderPass(**swapchain_->device(), &info, nullptr, &renderPass), "Could not create 3D Render Pass");
  renderPass_.set(renderPass, destroyRenderPass);
  LOG(trace) << "3D Render Pass created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createFrameBuffers()
{
  frameBuffers_.resize(swapchain_->imageCount());
  VkImageView attachments[3] {};
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    size_t attachmentIndex = 0;
    if (swapchain_->msaaLevel())   attachments[attachmentIndex++] = swapchain_->msaaBuffer();
    if (swapchain_->depthFormat()) attachments[attachmentIndex++] = swapchain_->depthBuffer();
    attachments[attachmentIndex++] = swapchain_->image(i).view;
    frameBuffers_[i] = std::make_unique<FrameBuffer>(this, attachments, attachmentIndex);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::onResize()
{
  base::onResize();
  VkImageView attachments[3] {};
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    size_t attachmentIndex = 0;
    if (swapchain_->msaaLevel())   attachments[attachmentIndex++] = swapchain_->msaaBuffer();
    if (swapchain_->depthFormat()) attachments[attachmentIndex++] = swapchain_->depthBuffer();
    attachments[attachmentIndex++] = swapchain_->image(i).view;
    frameBuffers_[i]->update(attachments, attachmentIndex);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createDescriptorPool(const Config::Resources& config)
{
  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = swapchain_->imageCount();
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = config.maxTextureCount() * swapchain_->imageCount();

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = 50; // TODO: FIX !

  VkDescriptorPool pool{};
  auto destroyPool = [device = **swapchain_->device()](auto pool) {
    vkDestroyDescriptorPool(device, pool, nullptr);
    LOG(trace) << "Descriptor Pool destroyed";
  };

  VULKAN_GUARD(vkCreateDescriptorPool(**swapchain_->device(), &poolInfo, nullptr, &pool), "Could not create descriptor pool");
  descriptorPool_.set(pool, destroyPool);
  LOG(trace) << "Descriptor Pool created";
}

// -----------------------------------------------------------------------------------------------------------------------------
PipelinePtr RenderPass3D::createPipeline(VkPipelineShaderStageCreateInfo* shaders, uint32_t count)
{
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
  const auto sampler           = getSampler(swapchain_->msaaLevel());
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
  info.layout              = layout_.pipeline();
  info.renderPass          = *renderPass_;
  info.subpass             = 0;
  info.basePipelineHandle  = VK_NULL_HANDLE;
  info.stageCount          = count;
  info.pStages             = shaders;

  auto pipeline       = std::make_unique<Pipeline>();
  pipeline->main      = vk::createPipeline(**swapchain_->device(), info);
  rasterizer          = getRasterizer(true);
  pipeline->wireframe = vk::createPipeline(**swapchain_->device(), info);
  return pipeline;
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createDescriptors(Pipeline* pipeline) const
{
  const auto device = swapchain_->device();
  const auto frames = swapchain_->imageCount();
  std::vector<VkDescriptorSetLayout> layouts(frames, layout_.texture());
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = *descriptorPool_;
  allocInfo.descriptorSetCount = frames;
  allocInfo.pSetLayouts = layouts.data();

  pipeline->descriptors.resize(frames);
  VULKAN_GUARD(vkAllocateDescriptorSets(**device, &allocInfo, pipeline->descriptors.data()),
    "Could not allocate Descriptor Sets");
}

} // !namespace vk
} // !namespace yaga
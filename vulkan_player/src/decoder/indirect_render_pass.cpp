#include "precompiled.h"
#include "indirect_render_pass.h"
#include "vulkan_renderer/vulkan_utils.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineInputAssemblyStateCreateInfo getPrimitives()
{
  VkPipelineInputAssemblyStateCreateInfo info{};
  info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  info.primitiveRestartEnable = VK_FALSE;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkViewport getViewport(const VkExtent2D& size)
{
  VkViewport viewport{};
  viewport.x        = 0.0f;
  viewport.y        = 0.0f;
  viewport.width    = static_cast<float>(size.width);
  viewport.height   = static_cast<float>(size.height);
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
  info.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  info.viewportCount = 1;
  info.pViewports    = &viewport;
  info.scissorCount  = 1;
  info.pScissors     = &scissors;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineRasterizationStateCreateInfo getRasterizer()
{
  VkPipelineRasterizationStateCreateInfo info{};
  info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  info.rasterizerDiscardEnable = VK_FALSE;
  info.depthClampEnable        = VK_FALSE;
  info.depthBiasEnable         = VK_FALSE;
  info.polygonMode             = VK_POLYGON_MODE_FILL;
  info.lineWidth               = 1.0f;
  info.cullMode                = VK_CULL_MODE_FRONT_BIT;
  info.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineMultisampleStateCreateInfo getSampler()
{
  VkPipelineMultisampleStateCreateInfo info{};
  info.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  info.sampleShadingEnable  = VK_FALSE;
  info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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
  info.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  info.logicOpEnable     = VK_FALSE;
  info.logicOp           = VK_LOGIC_OP_COPY;
  info.attachmentCount   = 1;
  info.pAttachments      = &blender;
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
  return info;
}

// -----------------------------------------------------------------------------------------------------------------------------
std::vector<VkClearValue> setClearValues(std::vector<VkClearValue>& clearValues)
{
  clearValues.resize(1);
  clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
  return clearValues;
}

// -----------------------------------------------------------------------------------------------------------------------------
void changeLayoutUndefinedGeneral(VkCommandBuffer command, VkImage image)
{
  VkImageMemoryBarrier barrier{};
  barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image                           = image;
  barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout                       = VK_IMAGE_LAYOUT_GENERAL;
  barrier.srcAccessMask                   = VK_ACCESS_NONE_KHR;
  barrier.dstAccessMask                   = VK_ACCESS_SHADER_WRITE_BIT;
  barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
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

} // !namespace

extern const unsigned char presentVertShader[];
extern const unsigned char presentFragShader[];
extern const size_t presentVertShaderSize;
extern const size_t presentFragShaderSize;

// -----------------------------------------------------------------------------------------------------------------------------
IndirectRenderPass::IndirectRenderPass(Swapchain* swapchain, VmaAllocator allocator, DecoderMiddlewarePtr decoder) :
  RenderPass(swapchain),
  allocator_(allocator),
  imageDescriptor_{}
{
  createDescriptorPool();
  setClearValues(clearValues_);
  createSampler();
  createDescriptorLayout();
  createPipelineLayout();
  createRenderPass();
  createShaders();
  createPipeline();
  createImage();
  createDescriptor();
  createFrameBuffers();
  decoder_ = createDecoderFrontend(swapchain, allocator, { image_.get() }, VK_IMAGE_LAYOUT_GENERAL, decoder);
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::render(uint32_t imageIndex)
{
  decoder_->decode(0);
  const auto command = frameBuffers_[imageIndex]->command();
  const auto viewport = getViewport(renderArea_.extent);
  vkCmdSetViewport       (command, 0, 1, &viewport);
  vkCmdSetScissor        (command, 0, 1, &renderArea_);
  vkCmdBindPipeline      (command, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline_);
  vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout_, 0, 1, &imageDescriptor_, 0, nullptr);
  vkCmdDraw              (command, 3, 1, 0, 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::onResize()
{
  base::onResize();
  createImage();
  updateDescriptor();
  updateFrameBuffers();
  decoder_->resize({ image_.get() }, swapchain_->resolution());
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createDescriptorPool()
{
  VkDescriptorPoolSize size{};
  size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  size.descriptorCount = 1;

  VkDescriptorPoolCreateInfo info{};
  info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  info.poolSizeCount = 1;
  info.pPoolSizes    = &size;
  info.maxSets       = 1;
  descriptorPool_ = vk::createDescriptorPool(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createSampler()
{
  VkSamplerCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.unnormalizedCoordinates = VK_FALSE;
  info.magFilter        = VK_FILTER_NEAREST;
  info.minFilter        = VK_FILTER_NEAREST;
  info.addressModeU     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeV     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeW     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.anisotropyEnable = VK_FALSE;
  info.maxAnisotropy    = 0;
  info.borderColor      = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info.compareEnable    = VK_FALSE;
  info.compareOp        = VK_COMPARE_OP_NEVER; 
  info.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_NEAREST;
  info.minLod           = 0;
  info.maxLod           = 0;
  sampler_ = vk::createSampler(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createDescriptorLayout()
{
  VkDescriptorSetLayoutBinding binding{};
  binding.binding            = 0;
  binding.descriptorCount    = 1;
  binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  binding.pImmutableSamplers = &*sampler_;
  binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = 1;
  info.pBindings    = &binding;
  descriptorLayout_ = createLayout(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createPipelineLayout()
{
  VkPipelineLayoutCreateInfo info{};
  info.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = 1;
  info.pSetLayouts    = &*descriptorLayout_;
  pipelineLayout_ = createLayout(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createRenderPass()
{
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format         = swapchain_->imageFormat();
  colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment  = 0;
  colorAttachmentRef.layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments    = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass    = 0;
  dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo info{};
  info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.attachmentCount = 1;
  info.pAttachments    = &colorAttachment;
  info.subpassCount    = 1;
  info.pSubpasses      = &subpass;
  info.dependencyCount = 1;
  info.pDependencies   = &dependency;
  renderPass_ = vk::createRenderPass(**swapchain_->device(), info); 
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createShaders()
{
  const auto device = **swapchain_->device();
  
  VkShaderModuleCreateInfo info{};
  info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.codeSize = presentVertShaderSize;
  info.pCode    = reinterpret_cast<const uint32_t*>(presentVertShader);
  vertexShader_ = createShader(device, info);

  info.codeSize = presentFragShaderSize;
  info.pCode    = reinterpret_cast<const uint32_t*>(presentFragShader);
  fragmentShader_ = createShader(device, info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createPipeline()
{
  VkPipelineVertexInputStateCreateInfo vertexAttributes{};
  vertexAttributes.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  std::array<VkPipelineShaderStageCreateInfo, 2> shaders {
    getShaderStageInfo(*vertexShader_,   VK_SHADER_STAGE_VERTEX_BIT,   "main"),
    getShaderStageInfo(*fragmentShader_, VK_SHADER_STAGE_FRAGMENT_BIT, "main")
  };

  std::array<VkDynamicState, 2> dynamicStates {
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
  const auto rasterizer        = getRasterizer();
  const auto sampler           = getSampler();
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
  info.layout              = *pipelineLayout_;
  info.renderPass          = *renderPass_;
  info.subpass             = 0;
  info.basePipelineHandle  = VK_NULL_HANDLE;
  info.stageCount          = 2;
  info.pStages             = shaders.data();
  pipeline_ = vk::createPipeline(**swapchain_->device(), info);  
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createImage()
{
  VkImageCreateInfo info{};
  info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType     = VK_IMAGE_TYPE_2D;
  info.extent.width  = static_cast<uint32_t>(swapchain_->resolution().width);
  info.extent.height = static_cast<uint32_t>(swapchain_->resolution().height);
  info.extent.depth  = 1;
  info.mipLevels     = 1;
  info.arrayLayers   = 1;
  info.format        = VK_FORMAT_R8G8B8A8_UNORM;
  info.tiling        = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage         = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  info.samples       = VK_SAMPLE_COUNT_1_BIT;
  info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format   = info.format;
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
  viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
  viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
  viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
  viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel   = 0;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount     = 1;
  viewInfo.subresourceRange.levelCount     = info.mipLevels;

  image_ = std::make_unique<Image>(swapchain_->device(), allocator_, info, viewInfo);
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createDescriptor()
{
  VkDescriptorSetAllocateInfo info{};
  info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  info.descriptorPool     = *descriptorPool_;
  info.descriptorSetCount = 1;
  info.pSetLayouts        = &*descriptorLayout_;
  imageDescriptor_ = vk::createDescriptor(**swapchain_->device(), info);
  updateDescriptor();
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::updateDescriptor()
{
  VkDescriptorImageInfo info{};
  info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  info.sampler     = *sampler_;
  info.imageView   = image_->view();

  VkWriteDescriptorSet writer{};
  writer.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writer.dstSet          = imageDescriptor_;
  writer.dstBinding      = 0;
  writer.dstArrayElement = 0;
  writer.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writer.descriptorCount = 1;
  writer.pImageInfo      = &info;
  vk::updateDescriptor(**swapchain_->device(), &writer, 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::createFrameBuffers()
{
  const size_t count = swapchain_->imageCount();
  frameBuffers_.resize(count);
  for (size_t i = 0; i < count; ++i) {
    auto view = swapchain_->image(i).view;
    frameBuffers_[i] = std::make_unique<FrameBuffer>(this, &view, 1);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void IndirectRenderPass::updateFrameBuffers()
{
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    auto view = swapchain_->image(i).view;
    frameBuffers_[i]->update(&view, 1);
  }
}

} // !namespace vk
} // !namespace yaga
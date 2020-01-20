#include "precompiled.h"
#include "material.h"
#include "shader.h"
#include "image_view.h"
#include "engine/vertex.h"

namespace yaga
{
namespace
{

// -------------------------------------------------------------------------------------------------------------------------
VkVertexInputBindingDescription getVertexBindingDescription()
{
  VkVertexInputBindingDescription bindingDescription = {};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

// -------------------------------------------------------------------------------------------------------------------------
std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescriptions()
{
  std::array<VkVertexInputAttributeDescription, 3> desc = {};

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
VkPipelineVertexInputStateCreateInfo getVertexAttributes()
{
  auto bindingDescription = getVertexBindingDescription();
  auto attributeDescriptions = getVertexAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  info.vertexBindingDescriptionCount = 1;
  info.pVertexBindingDescriptions = &bindingDescription;
  info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  info.pVertexAttributeDescriptions = attributeDescriptions.data();
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineInputAssemblyStateCreateInfo getPrimitives() 
{
  VkPipelineInputAssemblyStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  info.primitiveRestartEnable = VK_FALSE;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkViewport getViewport(const VkExtent2D& size)
{
  VkViewport viewport = {};
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
  VkRect2D scissors = {};
  scissors.offset = { 0, 0 };
  scissors.extent = size;
  return scissors;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineViewportStateCreateInfo getViewportState(const VkViewport& viewport, const VkRect2D& scissors)
{
  VkPipelineViewportStateCreateInfo info = {};
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
  VkPipelineRasterizationStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  info.depthClampEnable = VK_FALSE;
  info.rasterizerDiscardEnable = VK_FALSE;
  info.polygonMode = VK_POLYGON_MODE_FILL;
  info.lineWidth = 1.0f;
  //info.cullMode = VK_CULL_MODE_BACK_BIT;
  //info.frontFace = VK_FRONT_FACE_CLOCKWISE;

  info.cullMode = VK_CULL_MODE_BACK_BIT;
  info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

  info.depthBiasEnable = VK_FALSE;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineMultisampleStateCreateInfo getSampler(VkSampleCountFlagBits msaa)
{
  VkPipelineMultisampleStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  info.sampleShadingEnable = VK_TRUE;
  info.minSampleShading = 1.5f;
  info.rasterizationSamples = msaa;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineColorBlendAttachmentState getColorBlender()
{
  VkPipelineColorBlendAttachmentState info = {};
  info.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  info.blendEnable = VK_FALSE;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineColorBlendStateCreateInfo getColorBlendState(const VkPipelineColorBlendAttachmentState& blender)
{
  VkPipelineColorBlendStateCreateInfo info = {};
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
VkPipelineShaderStageCreateInfo getShaderStage(VkShaderModule module, VkShaderStageFlagBits shaderType)
{
  VkPipelineShaderStageCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = shaderType;
  info.module = module;
  info.pName = "main";
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineDepthStencilStateCreateInfo getDepthStencilState()
{
  VkPipelineDepthStencilStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  info.depthTestEnable = VK_TRUE;
  info.depthWriteEnable = VK_TRUE;
  info.depthCompareOp = VK_COMPARE_OP_LESS;
  info.depthBoundsTestEnable = VK_FALSE;
  info.stencilTestEnable = VK_FALSE;
  return info;
}

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
Material::Material(Device* device, Swapchain* swapchain, asset::Material* asset) :
  device_(device), vkDevice_(**device), swapchain_(swapchain), asset_(asset)
{
  createPipeline();
  createFramebuffers();
}

// -------------------------------------------------------------------------------------------------------------------------
void Material::createPipeline()
{
  Shader vertexShader(device_, asset_->vertexShader());
  Shader fragmentShader(device_, asset_->fragmentShader());
  VkPipelineShaderStageCreateInfo shaderStages[] = {
    getShaderStage(*vertexShader,  VK_SHADER_STAGE_VERTEX_BIT),
    getShaderStage(*fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
  };

  auto bindingDescription = getVertexBindingDescription();
  auto attributeDescriptions = getVertexAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexAttributes = {};
  vertexAttributes.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexAttributes.vertexBindingDescriptionCount = 1;
  vertexAttributes.pVertexBindingDescriptions = &bindingDescription;
  vertexAttributes.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexAttributes.pVertexAttributeDescriptions = attributeDescriptions.data();
  
  auto primitives = getPrimitives();
  auto viewport = getViewport(swapchain_->size());
  auto scissors = getScissors(swapchain_->size());
  auto viewportState = getViewportState(viewport, scissors);
  auto rasterizer = getRasterizer();
  auto sampler = getSampler(swapchain_->msaa());
  auto colorBlender = getColorBlender();
  auto colorBlendState = getColorBlendState(colorBlender);
  auto depthStencilState = getDepthStencilState();

  createRenderPass();

  VkGraphicsPipelineCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info.stageCount = 2;
  info.pStages = shaderStages;
  info.pVertexInputState = &vertexAttributes;
  info.pInputAssemblyState = &primitives;
  info.pViewportState = &viewportState;
  info.pRasterizationState = &rasterizer;
  info.pMultisampleState = &sampler;
  info.pColorBlendState = &colorBlendState;
  info.pDepthStencilState = &depthStencilState;
  info.layout = swapchain_->pipelineLayout();
  info.renderPass = *renderPass_;
  info.subpass = 0;
  info.basePipelineHandle = VK_NULL_HANDLE;

  auto destroyPipeline = [device = vkDevice_](auto pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
    LOG(trace) << "Pipeline destroyed";
  };
  VkPipeline pipeline;
  if (vkCreateGraphicsPipelines(vkDevice_, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline) != VK_SUCCESS) {
    THROW("Could not create Pipeline");
  }
  pipeline_.set(pipeline, destroyPipeline);
}

// -------------------------------------------------------------------------------------------------------------------------
void Material::createRenderPass()
{
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = swapchain_->imageFormat();
  colorAttachment.samples = swapchain_->msaa();
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = swapchain_->depthFormat();
  depthAttachment.samples = swapchain_->msaa();
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription msaaAttachment = {};
  msaaAttachment.format = swapchain_->imageFormat();
  msaaAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  msaaAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  msaaAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  msaaAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  msaaAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  msaaAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  msaaAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference msaaAttachmentRef = {};
  msaaAttachmentRef.attachment = 2;
  msaaAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  subpass.pResolveAttachments = &msaaAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, msaaAttachment };
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  auto device = **device_;
  auto destroyRenderPass = [device](auto renderPass) {
    vkDestroyRenderPass(device, renderPass, nullptr);
    LOG(trace) << "Render Pass destroyed";
  };
  VkRenderPass renderPass;
  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
    THROW("Could not create render pass");
  }
  renderPass_.set(renderPass, destroyRenderPass);
  LOG(trace) << "Render Pass created";
}

// -------------------------------------------------------------------------------------------------------------------------
void Material::createFramebuffers()
{
  auto device = **device_;
  auto destroyFrameBuffer = [device](auto frameBuffer) {
    vkDestroyFramebuffer(device, frameBuffer, nullptr);
    LOG(trace) << "Framebuffer destroyed";
  };
  const auto& frame = swapchain_->frames();
  frameBuffers_.resize(frame.size());
  frameBufferRefs_.resize(frame.size());
  for (size_t i = 0; i < frame.size(); i++)
  {
    std::array<VkImageView, 3> attachments = { swapchain_->renderTarget(), swapchain_->depthBuffer(), **frame[i] };
    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = *renderPass_;
    info.attachmentCount = static_cast<uint32_t>(attachments.size());
    info.pAttachments = attachments.data();
    info.width = swapchain_->size().width;
    info.height = swapchain_->size().height;
    info.layers = 1;
    VkFramebuffer frameBuffer;
    if (vkCreateFramebuffer(device, &info, nullptr, &frameBuffer) != VK_SUCCESS) {
      THROW("Could not create framebuffer");
    }
    frameBuffers_[i].set(frameBuffer, destroyFrameBuffer);
    frameBufferRefs_[i] = frameBuffer;
  }
}

} // !namespace yaga
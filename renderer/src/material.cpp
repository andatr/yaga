#include "precompiled.h"
#include "material.h"
#include "shader.h"
#include "image_view.h"
#include "asset/vertex.h"

namespace yaga
{
namespace
{

// -------------------------------------------------------------------------------------------------------------------------
VkVertexInputBindingDescription VertexBindingDescription()
{
  VkVertexInputBindingDescription bindingDescription = {};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

// -------------------------------------------------------------------------------------------------------------------------
std::array<VkVertexInputAttributeDescription, 3> VertexAttributeDescriptions()
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
VkPipelineVertexInputStateCreateInfo VertexAttributes()
{
  auto bindingDescription = VertexBindingDescription();
  auto attributeDescriptions = VertexAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  info.vertexBindingDescriptionCount = 1;
  info.pVertexBindingDescriptions = &bindingDescription;
  info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  info.pVertexAttributeDescriptions = attributeDescriptions.data();
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineInputAssemblyStateCreateInfo Primitives() 
{
  VkPipelineInputAssemblyStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  info.primitiveRestartEnable = VK_FALSE;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkViewport Viewport(const VkExtent2D& size)
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
VkRect2D Scissors(const VkExtent2D& size)
{
  VkRect2D scissors = {};
  scissors.offset = { 0, 0 };
  scissors.extent = size;
  return scissors;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineViewportStateCreateInfo ViewportState(const VkViewport& viewport, const VkRect2D& scissors)
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
VkPipelineRasterizationStateCreateInfo Rasterizer()
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
VkPipelineMultisampleStateCreateInfo Sampler()
{
  VkPipelineMultisampleStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  info.sampleShadingEnable = VK_FALSE;
  info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineColorBlendAttachmentState ColorBlender()
{
  VkPipelineColorBlendAttachmentState info = {};
  info.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  info.blendEnable = VK_FALSE;
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineColorBlendStateCreateInfo ColorBlendState(const VkPipelineColorBlendAttachmentState& blender)
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
VkPipelineShaderStageCreateInfo ShaderStage(VkShaderModule module, VkShaderStageFlagBits shaderType)
{
  VkPipelineShaderStageCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = shaderType;
  info.module = module;
  info.pName = "main";
  return info;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineDepthStencilStateCreateInfo DepthStencilState()
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
Material::Material(Device* device, VideoBuffer* videoBuffer, asset::Material* asset)
{
  CreatePipeline(device, videoBuffer, asset);
  CreateFramebuffers(device->Logical(), videoBuffer);
}

// -------------------------------------------------------------------------------------------------------------------------
void Material::CreatePipeline(Device* device, VideoBuffer* videoBuffer, asset::Material* asset)
{
  const auto logicalDevice = device->Logical();

  Shader vertexShader(logicalDevice, asset->VertexShader());
  Shader fragmentShader(logicalDevice, asset->FragmentShader());
  VkPipelineShaderStageCreateInfo shaderStages[] = {
    ShaderStage(vertexShader.ShaderModule(),  VK_SHADER_STAGE_VERTEX_BIT),
    ShaderStage(fragmentShader.ShaderModule(), VK_SHADER_STAGE_FRAGMENT_BIT)
  };

  auto bindingDescription = VertexBindingDescription();
  auto attributeDescriptions = VertexAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexAttributes = {};
  vertexAttributes.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexAttributes.vertexBindingDescriptionCount = 1;
  vertexAttributes.pVertexBindingDescriptions = &bindingDescription;
  vertexAttributes.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexAttributes.pVertexAttributeDescriptions = attributeDescriptions.data();
  
  auto primitives = Primitives();
  auto viewport = Viewport(videoBuffer->Size());
  auto scissors = Scissors(videoBuffer->Size());
  auto viewportState = ViewportState(viewport, scissors);
  auto rasterizer = Rasterizer();
  auto sampler = Sampler();
  auto colorBlender = ColorBlender();
  auto colorBlendState = ColorBlendState(colorBlender);
  auto depthStencilState = DepthStencilState();

  CreateRenderPass(logicalDevice, videoBuffer->ImageFormat(), videoBuffer->DepthFormat());

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
  info.layout = videoBuffer->PipelineLayout();
  info.renderPass = *renderPass_;
  info.subpass = 0;
  info.basePipelineHandle = VK_NULL_HANDLE;

  auto destroyPipeline = [logicalDevice](auto pipeline) {
    vkDestroyPipeline(logicalDevice, pipeline, nullptr);
    LOG(trace) << "Pipeline destroyed";
  };
  VkPipeline pipeline;
  if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline) != VK_SUCCESS) {
    THROW("Could not create Pipeline");
  }
  pipeline_.Assign(pipeline, destroyPipeline);
}

// -------------------------------------------------------------------------------------------------------------------------
void Material::CreateRenderPass(VkDevice device, VkFormat imageFormat, VkFormat depthFormat)
{
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = depthFormat;
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  auto destroyRenderPass = [device](auto renderPass) {
    vkDestroyRenderPass(device, renderPass, nullptr);
    LOG(trace) << "Render Pass destroyed";
  };
  VkRenderPass renderPass;
  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
    THROW("Could not create render pass");
  }
  renderPass_.Assign(renderPass, destroyRenderPass);
  LOG(trace) << "Render Pass created";
}

// -------------------------------------------------------------------------------------------------------------------------
void Material::CreateFramebuffers(VkDevice device, VideoBuffer* videoBuffer)
{
  auto destroyFrameBuffer = [device](auto frameBuffer) {
    vkDestroyFramebuffer(device, frameBuffer, nullptr);
    LOG(trace) << "Framebuffer destroyed";
  };
  const auto& frame = videoBuffer->Frames();
  frameBuffers_.resize(frame.size());
  frameBufferRefs_.resize(frame.size());
  for (size_t i = 0; i < frame.size(); i++)
  {
    std::array<VkImageView, 2> attachments = { **frame[i], videoBuffer->DepthBuffer() };
    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = *renderPass_;
    info.attachmentCount = static_cast<uint32_t>(attachments.size());
    info.pAttachments = attachments.data();
    info.width = videoBuffer->Size().width;
    info.height = videoBuffer->Size().height;
    info.layers = 1;
    VkFramebuffer frameBuffer;
    if (vkCreateFramebuffer(device, &info, nullptr, &frameBuffer) != VK_SUCCESS) {
      THROW("Could not create framebuffer");
    }
    frameBuffers_[i].Assign(frameBuffer, destroyFrameBuffer);
    frameBufferRefs_[i] = frameBuffer;
  }
}

} // !namespace yaga
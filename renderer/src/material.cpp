#include "precompiled.h"
#include "material.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"

namespace yaga
{
namespace
{

// -------------------------------------------------------------------------------------------------------------------------
VkVertexInputBindingDescription getBindingDescription()
{
  VkVertexInputBindingDescription bindingDescription = {};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

// -------------------------------------------------------------------------------------------------------------------------
std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
{
  std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, pos);

  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);

  return attributeDescriptions;
}

// -------------------------------------------------------------------------------------------------------------------------
VkPipelineVertexInputStateCreateInfo VertexAttributes()
{
  auto bindingDescription = getBindingDescription();
  auto attributeDescriptions = getAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  info.vertexBindingDescriptionCount = static_cast<uint32_t>(VertexDescription.size());
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
VkViewport Viewport(const VkExtent2D& resolution)
{
  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)resolution.width;
  viewport.height = (float)resolution.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  return viewport;
}

// -------------------------------------------------------------------------------------------------------------------------
VkRect2D Scissors(const VkExtent2D& resolution)
{
  VkRect2D scissors = {};
  scissors.offset = { 0, 0 };
  scissors.extent = resolution;
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
  info.cullMode = VK_CULL_MODE_BACK_BIT;
  info.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
Material::Material(Device* device, VideoBuffer* videoBuffer, VkCommandPool commandPool, asset::Material* asset)
{
	resolution_ = videoBuffer->Resolution();
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
    
  //auto vertexAttributes = VertexAttributes();
    
  auto bindingDescription = getBindingDescription();
  auto attributeDescriptions = getAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexAttributes = {};
  vertexAttributes.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexAttributes.vertexBindingDescriptionCount = 1;
  vertexAttributes.pVertexBindingDescriptions = &bindingDescription;
  vertexAttributes.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexAttributes.pVertexAttributeDescriptions = attributeDescriptions.data();
    

  auto primitives = Primitives();
  auto viewport = Viewport(videoBuffer->Resolution());
  auto scissors = Scissors(videoBuffer->Resolution());
  auto viewportState = ViewportState(viewport, scissors);
  auto rasterizer = Rasterizer();
  auto sampler = Sampler();
  auto colorBlender = ColorBlender();
  auto colorBlendState = ColorBlendState(colorBlender);

  CreateLayout(logicalDevice);
  CreateRenderPass(logicalDevice, videoBuffer->ImageFormat());

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
  info.layout = *layout_;
  info.renderPass = *renderPass_;
  info.subpass = 0;
  info.basePipelineHandle = VK_NULL_HANDLE;

  auto destroyPipeline = [logicalDevice](auto pipeline) {
    vkDestroyPipeline(logicalDevice, pipeline, nullptr);
    LOG(trace) << "Pipeline deleted";
  };
  VkPipeline pipeline;
  if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline) != VK_SUCCESS) {
    THROW("Could not create Pipeline");
  }
  pipeline_.Assign(pipeline, destroyPipeline);
}

// -------------------------------------------------------------------------------------------------------------------------
void Material::CreateLayout(VkDevice device)
{
  VkPipelineLayoutCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = 0;
  info.pushConstantRangeCount = 0;

  auto destroyLayout = [device](auto layout) {
    vkDestroyPipelineLayout(device, layout, nullptr);
    LOG(trace) << "Pipeline Layout deleted";
  };
  VkPipelineLayout layout;
  if (vkCreatePipelineLayout(device, &info, nullptr, &layout) != VK_SUCCESS) {
    THROW("Could not create Pipeline Layout");
  }
  layout_.Assign(layout, destroyLayout);
  LOG(trace) << "Pipeline Layout created";
}

// -------------------------------------------------------------------------------------------------------------------------
void Material::CreateRenderPass(VkDevice device, VkFormat imageFormat)
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

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  auto destroyRenderPass = [device](auto renderPass) {
    vkDestroyRenderPass(device, renderPass, nullptr);
    LOG(trace) << "Render Pass deleted";
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
    LOG(trace) << "Framebuffer deleted";
  };
  const auto& textures = videoBuffer->Textures();
  frameBuffers_.Resize(textures.size());
  frameBufferRefs_.resize(textures.size());
  for (size_t i = 0; i < textures.size(); i++)
  {
    VkImageView attachments[] = { textures[i]->ImageView() };
    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = *renderPass_;
    info.attachmentCount = 1;
    info.pAttachments = attachments;
    info.width = videoBuffer->Resolution().width;
    info.height = videoBuffer->Resolution().height;
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
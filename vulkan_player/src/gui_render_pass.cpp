#include "precompiled.h"
#include "gui_render_pass.h"
#include "vulkan_renderer/vulkan_utils.h"
#include "utility/glm.h"

namespace yaga {
namespace vk {
namespace {

constexpr float BUTTON_SIZE = 40.0f;
constexpr float SEARCH_SIZE = 40.0f;
constexpr float OFFSET      = 20.0f;

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
std::array<VkVertexInputAttributeDescription, 1> getVertexAttributeDescriptions()
{
  std::array<VkVertexInputAttributeDescription, 1> desc{};
  desc[0].binding  = 0;
  desc[0].location = 0;
  desc[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
  desc[0].offset   = offsetof(Vertex, position);
  return desc;
}

// -----------------------------------------------------------------------------------------------------------------------------
void updateIndices(std::vector<uint16_t>& indices)
{
  indices.resize(9 * 6);
  uint16_t index = 0;
  for (size_t i = 0; i < indices.size(); i += 6, index += 4) {
    indices[i    ] = index;
    indices[i + 1] = index + 1;
    indices[i + 2] = index + 2;
    indices[i + 3] = index + 2;
    indices[i + 4] = index + 3;
    indices[i + 5] = index;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void makeButton(std::vector<Vertex>& vertices, int& index, float& x, float& y)
{
  vertices[index++].position = { x,               y              , 1.0f };
  vertices[index++].position = { x + BUTTON_SIZE, y              , 1.0f };
  vertices[index++].position = { x + BUTTON_SIZE, y + BUTTON_SIZE, 1.0f };
  vertices[index++].position = { x,               y + BUTTON_SIZE, 1.0f };
  x += BUTTON_SIZE + OFFSET;
}

// -----------------------------------------------------------------------------------------------------------------------------
void makeSearchBar(std::vector<Vertex>& vertices, float width, int& index, float& x, float& y)
{
  vertices[index++].position = {          x, y              , 1.0f };
  vertices[index++].position = { 100.0f + x, y              , 1.0f };
  vertices[index++].position = { 100.0f + x, y + SEARCH_SIZE, 1.0f };
  vertices[index++].position = {          x, y + SEARCH_SIZE, 1.0f };
  vertices[index++].position = { 100.0f + x, y              , 1.0f };
  vertices[index++].position = { width  - x, y              , 1.0f };
  vertices[index++].position = { width  - x, y + SEARCH_SIZE, 1.0f };
  vertices[index++].position = { 100.0f + x, y + SEARCH_SIZE, 1.0f };
  y += SEARCH_SIZE + OFFSET;
}

// -----------------------------------------------------------------------------------------------------------------------------
void normalize(std::vector<Vertex>& vertices, uint32_t width, uint32_t height)
{
  const float w = width  / 2.0f;
  const float h = height / 2.0f;
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertices[i].position.x = vertices[i].position.x / w - 1.0f;
    vertices[i].position.y = vertices[i].position.y / h - 1.0f;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void updateVertices(std::vector<Vertex>& vertices, float width, float height)
{
  vertices.resize(9 * 4);
  int index = 0;
  float x = OFFSET;
  float y = height - BUTTON_SIZE - SEARCH_SIZE - 2.0f * OFFSET;
  makeSearchBar(vertices, width, index, x, y);
  makeButton   (vertices,        index, x, y);
  makeButton   (vertices,        index, x, y);
  makeButton   (vertices,        index, x, y);
  x = width - 4.0f * (BUTTON_SIZE + OFFSET);
  makeButton   (vertices,        index, x, y);
  makeButton   (vertices,        index, x, y);
  makeButton   (vertices,        index, x, y);
  makeButton   (vertices,        index, x, y);
  normalize    (vertices, width, height);
}

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
  info.cullMode                = VK_CULL_MODE_BACK_BIT;
  info.frontFace               = VK_FRONT_FACE_CLOCKWISE;
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

extern const unsigned char guiVertShader[];
extern const unsigned char guiFragShader[];
extern const size_t guiVertShaderSize;
extern const size_t guiFragShaderSize;

// -----------------------------------------------------------------------------------------------------------------------------
GuiRenderPass::GuiRenderPass(Swapchain* swapchain, VmaAllocator allocator, Input* input) :
  RenderPass(swapchain),
  allocator_(allocator),
  input_(input),
  mouseDown_(false)
{
  setClearValues(clearValues_);
  createPipelineLayout();
  createRenderPass();
  createShaders();
  createPipeline();
  createFrameBuffers();
  createBuffers();
  updateVertices();
}

// -----------------------------------------------------------------------------------------------------------------------------
void GuiRenderPass::render(uint32_t imageIndex)
{
  const auto command = frameBuffers_[imageIndex]->command();
  const auto viewport = getViewport(renderArea_.extent);
  const auto indexCount = static_cast<uint32_t>(indices_.size());
  VkBuffer vertexBuffers[] = { **vertexBuffer_ };
  VkDeviceSize offsets[] = { 0 };
  vkCmdSetViewport       (command, 0, 1, &viewport);
  vkCmdSetScissor        (command, 0, 1, &renderArea_);
  vkCmdBindPipeline      (command, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline_);
  vkCmdBindVertexBuffers (command, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer   (command, **indexBuffer_, 0, VK_INDEX_TYPE_UINT16);
  vkCmdDrawIndexed       (command, indexCount, 1, 0, 0, 0);

  if (input_->getState().mouseButtons[mouse_buttons::left].wasPressed) {
   
    LOG(warning) << "mouse pressed " << input_->getState().cursorPosition[0] << " " << input_->getState().cursorPosition[1];
  }
  if (input_->getState().mouseButtons[mouse_buttons::left].wasReleased) {
    
    LOG(warning) << "mouse released";
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void GuiRenderPass::onResize()
{
  base::onResize();
  updateFrameBuffers();
  updateVertices();  
}

// -----------------------------------------------------------------------------------------------------------------------------
void GuiRenderPass::createPipelineLayout()
{
  VkPipelineLayoutCreateInfo info{};
  info.sType      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayout_ = createLayout(**swapchain_->device(), info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void GuiRenderPass::createRenderPass()
{
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format         = swapchain_->imageFormat();
  colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
  colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
void GuiRenderPass::createShaders()
{
  const auto device = **swapchain_->device();
  
  VkShaderModuleCreateInfo info{};
  info.sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.codeSize   = guiVertShaderSize;
  info.pCode      = reinterpret_cast<const uint32_t*>(guiVertShader);
  vertexShader_   = createShader(device, info);

  info.codeSize   = guiFragShaderSize;
  info.pCode      = reinterpret_cast<const uint32_t*>(guiFragShader);
  fragmentShader_ = createShader(device, info);
}

// -----------------------------------------------------------------------------------------------------------------------------
void GuiRenderPass::createPipeline()
{
  const auto bindingDescription    = getVertexBindingDescription();
  const auto attributeDescriptions = getVertexAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexAttributes{};
  vertexAttributes.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexAttributes.vertexBindingDescriptionCount   = 1;
  vertexAttributes.pVertexBindingDescriptions      = &bindingDescription;
  vertexAttributes.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexAttributes.pVertexAttributeDescriptions    = attributeDescriptions.data();

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
void GuiRenderPass::createFrameBuffers()
{
  const size_t count = swapchain_->imageCount();
  frameBuffers_.resize(count);
  for (size_t i = 0; i < count; ++i) {
    auto view = swapchain_->image(i).view;
    frameBuffers_[i] = std::make_unique<FrameBuffer>(this, &view, 1);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void GuiRenderPass::updateFrameBuffers()
{
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    auto view = swapchain_->image(i).view;
    frameBuffers_[i]->update(&view, 1);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void GuiRenderPass::createBuffers()
{
  updateIndices(indices_);
  vk::updateVertices(
    vertices_, 
    static_cast<float>(swapchain_->resolution().width), 
    static_cast<float>(swapchain_->resolution().height)
  );

  const auto vertexSize = static_cast<VkDeviceSize>(sizeof(Vertex)   * vertices_.size());
  const auto indexSize  = static_cast<VkDeviceSize>(sizeof(uint16_t) * indices_.size());
  const auto stageSize  = std::max(vertexSize, indexSize);

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  allocInfo.flags = 0;

  VkBufferCreateInfo info{};
  info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size        = vertexSize;
  info.usage       = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  vertexBuffer_    = std::make_unique<Buffer>(allocator_, info, allocInfo);

  info.size        = indexSize;
  info.usage       = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  indexBuffer_     = std::make_unique<Buffer>(allocator_, info, allocInfo);

  info.size        = stageSize;
  info.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  allocInfo.usage  = VMA_MEMORY_USAGE_CPU_ONLY;
  allocInfo.flags  = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  stageBuffer_     = std::make_unique<Buffer>(allocator_, info, allocInfo);

  void* mappedData = nullptr;
  vmaMapMemory(allocator_, stageBuffer_->allocation(), &mappedData);
  memcpy(mappedData, indices_.data(), indexSize);
  vmaUnmapMemory(allocator_, stageBuffer_->allocation());
  swapchain_->device()->submitCommand([src = **stageBuffer_, dest = **indexBuffer_, size = indexSize](auto command) {
    copyBuffer(src, dest, size, command);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void GuiRenderPass::updateVertices()
{
  vk::updateVertices(vertices_, swapchain_->resolution().width, swapchain_->resolution().height);
  const auto size = static_cast<VkDeviceSize>(sizeof(Vertex) * vertices_.size());
  void* mappedData = nullptr;
  vmaMapMemory(allocator_, stageBuffer_->allocation(), &mappedData);
  memcpy(mappedData, vertices_.data(), size);
  vmaUnmapMemory(allocator_, stageBuffer_->allocation());
  swapchain_->device()->submitCommand([src = **stageBuffer_, dest = **vertexBuffer_, size](auto command) {
    copyBuffer(src, dest, size, command);
  });
}

} // !namespace vk
} // !namespace yaga
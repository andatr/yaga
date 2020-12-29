#include "precompiled.h"
#include "render_pass_3d.h"
#include "frame_buffer.h"
#include "uniform.h"

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

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
RenderPass3D::RenderPass3D(Swapchain* swapchain) :
  RenderPass(swapchain->device()),
  swapchain_(swapchain)
{
  finalStage_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  renderArea_ = {{ 0, 0 }, { swapchain->resolution().width, swapchain->resolution().height }};
  clearValues_.resize(2);
  clearValues_[0].color = { 0.4f, 0.4f, 0.4f, 1.0f };
  clearValues_[1].depthStencil = { 1.0f, 0 };
  createRenderPass();
  createUniformLayout();
  createTextureLayout();
  createPipelineLayout();
  createSampler();
  createFrameBuffers();
  resizeConnection_ = swapchain_->onResize(std::bind(&RenderPass3D::onResize, this));
}

// -----------------------------------------------------------------------------------------------------------------------------
RenderPass3D::~RenderPass3D()
{
  swapchain_->onResize(resizeConnection_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::onResize()
{
  renderArea_ = {{ 0, 0 }, { swapchain_->resolution().width, swapchain_->resolution().height }};
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    std::array<VkImageView, 3> attachments{
      swapchain_->msaaBuffer(),
      swapchain_->depthBuffer(),
      swapchain_->image(i)
    };
    frameBuffers_[i]->update(attachments.data(), attachments.size());
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
VkCommandBuffer RenderPass3D::beginRender(uint32_t image)
{
  const auto viewport = getViewport(renderArea_.extent);
  auto command = base::beginRender(image);
  vkCmdSetViewport(command, 0, 1, &viewport);
  vkCmdSetScissor (command, 0, 1, &renderArea_);
  return command;
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createRenderPass()
{
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format         = swapchain_->imageFormat();
  colorAttachment.samples        = swapchain_->msaa();
  colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format         = swapchain_->depthFormat();
  depthAttachment.samples        = swapchain_->msaa();
  depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription msaaAttachment{};
  msaaAttachment.format          = swapchain_->imageFormat();
  msaaAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
  msaaAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  msaaAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
  msaaAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  msaaAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  msaaAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
  msaaAttachment.finalLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment  = 0;
  colorAttachmentRef.layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment  = 1;
  depthAttachmentRef.layout      = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference msaaAttachmentRef{};
  msaaAttachmentRef.attachment   = 2;
  msaaAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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
  dependency.srcAccessMask = 0;
  dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {
    colorAttachment,
    depthAttachment,
    msaaAttachment
  };
  VkRenderPassCreateInfo info{};
  info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.attachmentCount = static_cast<uint32_t>(attachments.size());
  info.pAttachments    = attachments.data();
  info.subpassCount    = 1;
  info.pSubpasses      = &subpass;
  info.dependencyCount = 1;
  info.pDependencies   = &dependency;

  auto destroyRenderPass = [dev = **device_](auto renderPass) {
    vkDestroyRenderPass(dev, renderPass, nullptr);
    LOG(trace) << "3D Render Pass destroyed";
  };
  VkRenderPass renderPass;
  VULKAN_GUARD(vkCreateRenderPass(**device_, &info, nullptr, &renderPass), "Could not create 3D Render Pass");
  renderPass_.set(renderPass, destroyRenderPass);
  LOG(trace) << "3D Render Pass created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createFrameBuffers()
{
  frameBuffers_.resize(swapchain_->imageCount());
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    std::array<VkImageView, 3> attachments {
      swapchain_->msaaBuffer(),
      swapchain_->depthBuffer(),
      swapchain_->image(i)
    };
    frameBuffers_[i] = std::make_unique<FrameBuffer>(this, attachments.data(), attachments.size());
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createUniformLayout()
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

  auto destroyLayout = [device = **swapchain_->device()](auto layout) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    LOG(trace) << "Descriptor Set Layout destroyed";
  };

  VkDescriptorSetLayout layout;
  VULKAN_GUARD(vkCreateDescriptorSetLayout(**swapchain_->device(), &info, nullptr, &layout),
    "Could not create Uniform Descriptor Layout");
  uniformLayout_.set(layout, destroyLayout);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createTextureLayout()
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

  auto destroyLayout = [device = **swapchain_->device()](auto layout) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    LOG(trace) << "Descriptor Set Layout destroyed";
  };

  VkDescriptorSetLayout layout;
  VULKAN_GUARD(vkCreateDescriptorSetLayout(**swapchain_->device(), &info, nullptr, &layout),
    "Could not create material Descriptor Set Layout");
  textureLayout_.set(layout, destroyLayout);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createPipelineLayout()
{
  std::array<VkPushConstantRange, 1> pushConstants;
  pushConstants[0].offset     = 0;
  pushConstants[0].size       = sizeof(PushConstantVertex);
  pushConstants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  std::array<VkDescriptorSetLayout, 2> setLayouts = { *uniformLayout_, *textureLayout_ };
  VkPipelineLayoutCreateInfo info{};
  info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
  info.pPushConstantRanges    = pushConstants.data();
  info.setLayoutCount         = static_cast<uint32_t>(setLayouts.size());
  info.pSetLayouts            = setLayouts.data();

  auto destroyLayout = [device = **swapchain_->device()](auto layout) {
    vkDestroyPipelineLayout(device, layout, nullptr);
    LOG(trace) << "Pipeline Layout destroyed";
  };

  VkPipelineLayout layout;
  VULKAN_GUARD(vkCreatePipelineLayout(**swapchain_->device(), &info, nullptr, &layout), "Could not create Pipeline Layout");
  pipelineLayout_.set(layout, destroyLayout);
  LOG(trace) << "Pipeline Layout created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPass3D::createSampler()
{
  VkSamplerCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.unnormalizedCoordinates = VK_FALSE;
  info.magFilter        = VK_FILTER_LINEAR;
  info.minFilter        = VK_FILTER_LINEAR;
  info.addressModeU     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeV     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeW     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.anisotropyEnable = VK_TRUE;
  info.maxAnisotropy    = 16;
  info.borderColor      = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info.compareEnable    = VK_FALSE;
  info.compareOp        = VK_COMPARE_OP_ALWAYS;
  info.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  info.minLod           = 0;
  info.maxLod           = 0;

  auto destroySampler = [device = **swapchain_->device()](auto sampler) {
    vkDestroySampler(device, sampler, nullptr);
    LOG(trace) << "Texture Sampler destroyed";
  };
  VkSampler sampler;
  VULKAN_GUARD(vkCreateSampler(**swapchain_->device(), &info, nullptr, &sampler), "Could not create Texture Sampler");
  sampler_.set(sampler, destroySampler);
}

} // !namespace vk
} // !namespace yaga

#include "precompiled.h"
#include "render_pass_gui.h"
#include "frame_buffer.h"
#include "uniform.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
RenderPassGui::RenderPassGui(Swapchain* swapchain) :
  RenderPass(swapchain->device()),
  swapchain_(swapchain)
{
  finalStage_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  renderArea_ = {{ 0, 0 }, { swapchain->resolution().width, swapchain->resolution().height }};
  clearValues_.resize(1);
  clearValues_[0].color = { 0.7f, 0.7f, 0.7f, 1.0f };
  createRenderPass();
  createFrameBuffers();
  resizeConnection_ = swapchain_->onResize(std::bind(&RenderPassGui::onResize, this));
}

// -----------------------------------------------------------------------------------------------------------------------------
RenderPassGui::~RenderPassGui()
{
  swapchain_->onResize(resizeConnection_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::onResize()
{
  renderArea_ = {{ 0, 0 }, { swapchain_->resolution().width, swapchain_->resolution().height }};
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    std::array<VkImageView, 1> attachments {
      swapchain_->image(i)
    };
    frameBuffers_[i]->update(attachments.data(), attachments.size());
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::createRenderPass()
{
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format         = swapchain_->imageFormat();
  colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
  colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment  = 0;
  colorAttachmentRef.layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass   = {};
  subpass.pipelineBindPoint      = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount   = 1;
  subpass.pColorAttachments      = &colorAttachmentRef;

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
  info.attachmentCount = 1;
  info.pAttachments    = &colorAttachment;
  info.subpassCount    = 1;
  info.pSubpasses      = &subpass;
  info.dependencyCount = 1;
  info.pDependencies   = &dependency;

  auto destroyRenderPass = [dev = **device_](auto renderPass) {
    vkDestroyRenderPass(dev, renderPass, nullptr);
    LOG(trace) << "GUI Render Pass destroyed";
  };
  VkRenderPass renderPass;
  VULKAN_GUARD(vkCreateRenderPass(**device_, &info, nullptr, &renderPass), "Could not create GUI Render Pass");
  renderPass_.set(renderPass, destroyRenderPass);
  LOG(trace) << "GUI Render Pass created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::createFrameBuffers()
{
  frameBuffers_.resize(swapchain_->imageCount());
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    std::array<VkImageView, 1> attachments {
      swapchain_->image(i)
    };
    frameBuffers_[i] = std::make_unique<FrameBuffer>(this, attachments.data(), attachments.size());
  }
}

} // !namespace vk
} // !namespace yaga

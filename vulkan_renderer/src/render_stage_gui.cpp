#include "precompiled.h"
#include "render_stage_gui.h"
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "render_pass_gui.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
void checkResult(VkResult result)
{
  if (result != VK_SUCCESS) {
    THROW("UmGui error");
  }
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
RenderStageGui::RenderStageGui(Swapchain* swapchain, Window* window) :
  swapchain_(swapchain)
{
  createDescriptorPool();
  renderPass_ = std::make_unique<RenderPassGui>(swapchain_);
  initGui(window);
  uploadFonts();
}

// -----------------------------------------------------------------------------------------------------------------------------
RenderStageGui::~RenderStageGui()
{
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderStageGui::initGui(Window* window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(**window, true);
  ImGui_ImplVulkan_InitInfo info{};
  info.Instance        = window->instance();
  info.PhysicalDevice  = swapchain_->device()->physical();
  info.Device          = **swapchain_->device();
  info.QueueFamily     = swapchain_->device()->queueFamilies().graphics;
  info.Queue           = swapchain_->device()->graphicsQueue();
  info.DescriptorPool  = *descriptorPool_;
  info.MinImageCount   = swapchain_->info().minImageCount;
  info.ImageCount      = swapchain_->imageCount();
  info.CheckVkResultFn = checkResult;
  ImGui_ImplVulkan_Init(&info, **renderPass_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderStageGui::uploadFonts()
{
  swapchain_->device()->submitCommand([](auto command) {
    ImGui_ImplVulkan_CreateFontsTexture(command);
  });
  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

// -----------------------------------------------------------------------------------------------------------------------------
VkSemaphore RenderStageGui::render(Context*, uint32_t frame, VkSemaphore waitFor)
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::ShowDemoWindow();
  ImGui::Render();
  ImDrawData* draw_data = ImGui::GetDrawData();
  auto command = renderPass_->beginRender(frame);
  ImGui_ImplVulkan_RenderDrawData(draw_data, command);
  return renderPass_->finishRender(frame, { renderPass_->finalStage(), waitFor }).semaphore;
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderStageGui::createDescriptorPool()
{
  VkDescriptorPoolSize poolSizes[] = {
    { VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }
  };
  const uint32_t poolSizeCount = sizeof(poolSizes) / sizeof(poolSizes[0]);

  auto destroyPool = [device = **swapchain_->device()](auto pool) {
    vkDestroyDescriptorPool(device, pool, nullptr);
    LOG(trace) << "GUI Descriptor Pool destroyed";
  };

  VkDescriptorPoolCreateInfo info = {};
  info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  info.maxSets       = 1000 * poolSizeCount;
  info.poolSizeCount = poolSizeCount;
  info.pPoolSizes    = poolSizes;
  
  VkDescriptorPool pool;
  VULKAN_GUARD(vkCreateDescriptorPool(**swapchain_->device(), &info, nullptr, &pool), "Could not create GUI Command Pool");
  descriptorPool_.set(pool, destroyPool);
  LOG(trace) << "GUI Descriptor Pool created";
}

} // !namespace vk
} // !namespace yaga

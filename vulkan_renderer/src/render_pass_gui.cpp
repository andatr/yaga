#include "precompiled.h"
#include "vulkan_renderer/render_pass_gui.h"

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

// -----------------------------------------------------------------------------------------------------------------------------
std::vector<VkClearValue> getClearValues(const Config::Gui& config)
{
  std::vector<VkClearValue> clearValues(1);
  clearValues[0].color = { 
    config.clearColor().x,
    config.clearColor().y,
    config.clearColor().z,
    config.clearColor().w
  };
  return clearValues;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
RenderPassGui::RenderPassGui(Swapchain* swapchain, Window* window, ConfigPtr config, GuiProc proc) :
  RenderPass(swapchain),
  config_(config),
  proc_(proc)
{
  clearValues_ = getClearValues(config_->gui());
  createDescriptorPool();
  createRenderPass();
  createFrameBuffers();
  initGui(window);
  uploadFonts();
}

// -----------------------------------------------------------------------------------------------------------------------------
RenderPassGui::~RenderPassGui()
{
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::saveConfig()
{
  ImGuiIO& io = ImGui::GetIO();
  //if (io.WantSaveIniSettings) {
    size_t size = 0;
    const char* ptr = ImGui::SaveIniSettingsToMemory(&size);
    io.WantSaveIniSettings = false;
    std::string str(ptr, size);
    config_->gui().imGuiSettings(str);
  //}
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::beginRender(uint32_t imageIndex)
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  proc_();
  ImGui::Render();
  base::beginRender(imageIndex);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::render(uint32_t imageIndex)
{
  ImDrawData* drawData = ImGui::GetDrawData();
  const auto   command = frameBuffers_[imageIndex]->command();
  ImGui_ImplVulkan_RenderDrawData(drawData, command);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::initGui(Window* window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  const std::string& settings = config_->gui().imGuiSettings();
  if (!settings.empty()) {
    ImGui::LoadIniSettingsFromMemory(settings.c_str(), settings.size() + 1);
  }
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(**window, true);
  const auto device = swapchain_->device();
  ImGui_ImplVulkan_InitInfo info{};
  info.Instance        = window->instance();
  info.PhysicalDevice  = device->physical();
  info.Device          = **device;
  info.QueueFamily     = device->queueFamilies().graphics;
  info.Queue           = device->graphicsQueue();
  info.DescriptorPool  = *descriptorPool_;
  info.MinImageCount   = swapchain_->info().minImageCount;
  info.ImageCount      = swapchain_->imageCount();
  info.CheckVkResultFn = checkResult;
  ImGui_ImplVulkan_Init(&info, *renderPass_);
  
  // TODO: FIX!!!
  
  //IMGUI_API ImFont* AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
  //IMGUI_API ImFont* AddFontFromMemoryTTF(void* font_data, int font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); 
  // Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas. 
  // Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
  io.Fonts->AddFontFromFileTTF(R"(C:\Projects\cpp\yaga\src\data\model_viewer\DroidSans.ttf)", 16.0f);
  //io.IniFilename = config.guiConfigPath();

  ImFontConfig cconfig{};
  cconfig.MergeMode = true;
  static const ImWchar icon_ranges[] = { 0xe005, 0xf8ff, 0 };
  io.Fonts->AddFontFromFileTTF(R"(C:\Projects\cpp\yaga\src\data\model_viewer\fa-solid-900.ttf)", 16.0f, &cconfig, icon_ranges);
  ImGuiFreeType::BuildFontAtlas(io.Fonts, ImGuiFreeType::RasterizerFlags::LightHinting);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::uploadFonts()
{
  swapchain_->device()->submitCommand([](auto command) {
    ImGui_ImplVulkan_CreateFontsTexture(command);
  });
  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::createDescriptorPool()
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
  
  const auto device = **swapchain_->device();
  auto destroyPool = [device](auto pool) {
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
  VULKAN_GUARD(vkCreateDescriptorPool(device, &info, nullptr, &pool), "Could not create GUI Command Pool");
  descriptorPool_.set(pool, destroyPool);
  LOG(trace) << "GUI Descriptor Pool created";
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
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments    = &colorAttachmentRef;

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

  auto destroyRenderPass = [dev = **swapchain_->device()](auto renderPass) {
    vkDestroyRenderPass(dev, renderPass, nullptr);
    LOG(trace) << "GUI Render Pass destroyed";
  };
  VkRenderPass renderPass;
  VULKAN_GUARD(vkCreateRenderPass(**swapchain_->device(), &info, nullptr, &renderPass), "Could not create GUI Render Pass");
  renderPass_.set(renderPass, destroyRenderPass);
  LOG(trace) << "GUI Render Pass created";
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::createFrameBuffers()
{
  frameBuffers_.resize(swapchain_->imageCount());
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    std::array<VkImageView, 1> attachments {
      swapchain_->image(i).view
    };
    frameBuffers_[i] = std::make_unique<FrameBuffer>(this, attachments.data(), attachments.size());
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderPassGui::onResize()
{
  base::onResize();
  for (size_t i = 0; i < frameBuffers_.size(); ++i) {
    std::array<VkImageView, 1> attachments {
      swapchain_->image(i).view
    };
    frameBuffers_[i]->update(attachments.data(), attachments.size());
  }
}

} // !namespace vk
} // !namespace yaga

#include "precompiled.h"
#include "vulkan_renderer/platform.h"
#include "vulkan_renderer/device.h"
#include "vulkan_renderer/renderer.h"
#include "vulkan_renderer/render_pass_3d.h"
#include "vulkan_renderer/render_pass_gui.h"
#include "vulkan_renderer/swapchain.h"

DISABLE_WARNINGS
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
ENABLE_WARNINGS

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
PlatformPtr createPlatform()
{
  return std::make_unique<vk::Platform>();
}

namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Platform::Platform() :
  running_(false),
  app_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Platform::~Platform()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::run(ApplicationPtr app)
{
  if (running_) return;
  running_ = true;
  app_ = app;
  auto config = app->config();
  SwapchainParams swParams{};
  initVulkan(config, swParams, instance_, window_, debugMessenger_, device_, allocator_, swapchain_);
  renderer_ = std::make_unique<Renderer>(swapchain_.get());
  auto renderPass3D  = std::make_shared<RenderPass3D>(swapchain_.get(), *allocator_, config);
  auto renderPassGui = std::make_shared<RenderPassGui>(
    swapchain_.get(),
    window_.get(),
    config,
    [app]() { app->gui(); }
  );  
  renderer_->passes().push_back(renderPass3D);
  renderer_->passes().push_back(renderPassGui);
  context_ = std::make_unique<Context>(
    swapchain_.get(), 
    renderPass3D->cameraPool(),
    renderPass3D->meshPool(),
    renderPass3D->materialPool(),
    renderPass3D->rendererPool()
  );
  app->init(context_.get(), window_->input());
  loop();
  renderPassGui->saveConfig();
  app->shutdown();
  context_->clear();
  device_->waitIdle();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Platform::loop()
{
  while (true) {
    window_->update();
    context_->update();
    if (glfwWindowShouldClose(**window_)) {
      app_->stop();
      glfwSetWindowShouldClose(**window_, 0);
    }
    if (!app_->loop()) break;
    if (window_->minimised()) continue;
    if (window_->resized() || !renderer_->render()) {
      device_->waitIdle();
      window_->resized(false);
      swapchain_->resize(window_->size());
      app_->resize();
    }
  }
}

} // !namespace vk
} // !namespace yaga
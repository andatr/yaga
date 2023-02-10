#include "precompiled.h"
#include "application.h"
#include "gui_render_pass.h"
#include "decoder/async_decoder_middleware.h"
#include "decoder/decoder_render_pass.h"
#include "vulkan_renderer/render_pass_gui.h"
#include "vulkan_renderer/vulkan_utils.h"

DISABLE_WARNINGS
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
ENABLE_WARNINGS

namespace yaga {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
unsigned int getTaskPoolSize()
{
  auto size = std::thread::hardware_concurrency();
  if (size > 1) return size - 1;
  return 1;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Application::Application(ConfigPtr config, const std::string& filename) :
  config_  (config),
  filename_(filename),
  running_ (false),
  taskPool_(getTaskPoolSize()),
  docking_(this),
  guiContext_(this, &docking_)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Application::~Application()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::run()
{
  if (running_) return;
  running_ = true;
  init();
  loop();
  device_->waitIdle();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::init()
{
  initVulkan();
  createRenderer();
  renderGui(guiContext_, this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::initVulkan()
{
  vk::SwapchainParams params{};
  params.imageUsage = VK_IMAGE_USAGE_STORAGE_BIT;
  vk::initVulkan(config_, params, instance_, window_, debugMessenger_, device_, allocator_, swapchain_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::createRenderer()
{
  auto player = std::make_shared<AsyncDecoderMiddleware>(
    swapchain_->resolution().width,
    swapchain_->resolution().height
  );
  player_ = player;
  renderer_ = std::make_unique<vk::Renderer>(swapchain_.get());
  renderer_->passes().push_back(vk::createDecoderRenderPass(swapchain_.get(), *allocator_, player));
  renderer_->passes().push_back(std::make_shared<vk::GuiRenderPass>(swapchain_.get(), *allocator_, window_->input()));
  renderer_->passes().push_back(std::make_shared<vk::RenderPassGui>(
    swapchain_.get(),
    window_.get(),
    config_,
    [this]() { gui(); }
  ));
  player_->start(filename_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::loop()
{
  while (running_) {
    window_->update();
    if (glfwWindowShouldClose(**window_)) {
      glfwSetWindowShouldClose(**window_, 0);
      running_ = false;
    }
    if (window_->resized() || !(window_->minimised() ? updateMinimised() : update())) resize();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Application::update()
{
  return renderer_->render();
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Application::updateMinimised()
{
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::gui()
{
  docking_.update();
  dispatcher_.process();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::resize() 
{
  device_->waitIdle();
  window_->resized(false);
  swapchain_->resize(window_->size());
}

// -----------------------------------------------------------------------------------------------------------------------------
Dispatcher& Application::dispatcher()
{
  return dispatcher_;
}

// -----------------------------------------------------------------------------------------------------------------------------
boost::asio::thread_pool& Application::taskPool()
{
  return taskPool_;
}

// -----------------------------------------------------------------------------------------------------------------------------
ImVec2 Application::screenSize()
{
  return { 
    static_cast<float>(swapchain_->resolution().width), 
    static_cast<float>(swapchain_->resolution().height)
  };
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::open(const std::string& filename)
{
  player_->start(filename);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::exit()
{
  running_ = false;
}

} // !namespace yaga
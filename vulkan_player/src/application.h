#ifndef YAGA_VULKAN_PLAYER_SRC_APP
#define YAGA_VULKAN_PLAYER_SRC_APP

#include <memory>

#include "gui.h"
#include "player.h"
#include "vulkan_renderer/renderer.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/window.h"
#include "utility/auto_destructor.h"

namespace yaga {

class Application :
  public GuiContextApp,
  public GuiApp
{
public:
  explicit Application(ConfigPtr config, const std::string& filename);
  virtual ~Application();
  void run();
  // GuiContextApp
  ImVec2 screenSize() override;
  Dispatcher& dispatcher() override;
  boost::asio::thread_pool& taskPool() override;
  // GuiApp
  void open(const std::string& filename) override;
  void exit() override;
  Player* player() override { return player_.get(); }

private:
  void init();
  void initVulkan();
  void createRenderer();
  void loop();
  void resize();
  bool update();
  bool updateMinimised();
  void gui();

private:
  typedef AutoDestructor<VkDebugUtilsMessengerEXT> Messanger;
  typedef AutoDestructor<VmaAllocator> Allocator;
  typedef AutoDestructor<VkInstance> Instance;

private:
  ConfigPtr config_;
  std::string filename_;
  bool running_;
  Instance instance_;
  Messanger debugMessenger_;
  vk::WindowPtr window_;  
  vk::DevicePtr device_;
  Allocator allocator_;
  vk::SwapchainPtr swapchain_;
  boost::asio::thread_pool taskPool_;
  DockingContext docking_;
  GuiContext guiContext_;
  Dispatcher dispatcher_;
  vk::RendererPtr  renderer_;
  PlayerPtr player_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_APP

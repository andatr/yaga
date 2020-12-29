#ifndef YAGA_VULKAN_RENDERER_SRC_GUI
#define YAGA_VULKAN_RENDERER_SRC_GUI

#include <memory>

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
class Gui
{
public:
  explicit Gui();
  virtual ~Gui();

private:

};

typedef std::unique_ptr<Gui> GuiPtr;

} // namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_GUI

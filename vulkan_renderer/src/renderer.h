#ifndef YAGA_VULKAN_RENDERER_SRC_RENDERER
#define YAGA_VULKAN_RENDERER_SRC_RENDERER

#include <memory>
#include <vector>

#include "device.h"
#include "renderer3d.h"
#include "rendering_context.h"
#include "swapchain.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class Material;

class Renderer
{
public:
  explicit Renderer(Swapchain* swapchain, RenderingContext* context);
  void swapchain(Swapchain* swapchain) { swapchain_ = swapchain; }
  void render(uint32_t frame) const;

private:
  static void renderObject(Camera* camera, Renderer3D* object, VkCommandBuffer command, uint32_t frame);

private:
  Swapchain* swapchain_;
  RenderingContext* context_;
  uint32_t frames_;
};

typedef std::unique_ptr<Renderer> RendererPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDERER

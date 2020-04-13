#ifndef YAGA_VULKAN_RENDERER_SRC_RENDERER
#define YAGA_VULKAN_RENDERER_SRC_RENDERER

#include <memory>
#include <vector>

#include "device.h"
#include "swapchain.h"
#include "vulkan.h"
#include "engine/scene.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class Material;

class Renderer
{
public:
  explicit Renderer(Device* device, Swapchain* swapchain);
  void swapchain(Swapchain* swapchain);
  VkCommandBuffer render(Scene* scene, uint32_t frame);
private:  
  void allocateCommandBuffer();
  void renderModel(Object* object, VkCommandBuffer command, uint32_t frame) const;
private:
  Device* device_;
  VkDevice vkDevice_;
  Swapchain* swapchain_;
  uint32_t frames_;
  std::vector<VkCommandBuffer> commandBuffers_;
};

typedef std::unique_ptr<Renderer> RendererPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDERER

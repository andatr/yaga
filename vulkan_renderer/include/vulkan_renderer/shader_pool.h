#ifndef YAGA_VULKAN_RENDERER_SHADER
#define YAGA_VULKAN_RENDERER_SHADER

#include <memory>
#include <unordered_map>

#include "vulkan_renderer/device.h"
#include "vulkan_renderer/vulkan.h"
#include "assets/shader.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class ShaderPool
{
public:
  explicit ShaderPool(Device* device);
  ~ShaderPool();
  VkShaderModule get(assets::ShaderPtr asset);
  void clear();

private:
  Device* device_;
  std::unordered_map<assets::ShaderPtr, AutoDestructor<VkShaderModule>> shaders_;
};

typedef std::unique_ptr<ShaderPool> ShaderPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SHADER

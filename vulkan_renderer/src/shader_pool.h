#ifndef YAGA_VULKAN_RENDERER_SRC_SHADER
#define YAGA_VULKAN_RENDERER_SRC_SHADER

#include <memory>
#include <unordered_map>

#include "device.h"
#include "vulkan.h"
#include "assets/shader.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class ShaderPool
{
public:
  explicit ShaderPool(Device* device);
  ~ShaderPool();
  VkShaderModule get(assets::Shader* asset);
  void clear();

private:
  Device* device_;
  std::unordered_map<assets::Shader*, AutoDestructor<VkShaderModule>> shaders_;
};

typedef std::unique_ptr<ShaderPool> ShaderPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_SHADER

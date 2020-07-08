#ifndef YAGA_VULKAN_RENDERER_UNIFORM
#define YAGA_VULKAN_RENDERER_UNIFORM

#include <memory>
#include <vector>

#include "buffer.h"
#include "engine/camera.h"

namespace yaga {
namespace vk {

struct UniformObject
{
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
};

struct PushConstantVertex
{
  alignas(16) glm::mat4 model;
};

struct PushConstantFragment
{
  alignas(16) glm::vec4 ambient;
  alignas(16) glm::vec4 diffuse;
  alignas(16) glm::vec4 specular;
  alignas(4) float opacity;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_UNIFORM

#include "precompiled.h"
#include "shader.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Shader::Shader(Device* device, assets::Shader* asset)
{
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = asset->code().size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(asset->code().data());
  auto vkDevice = **device;
  auto destroyShader = [vkDevice](auto shader) {
    vkDestroyShaderModule(vkDevice, shader, nullptr);
    LOG(trace) << "Shader destroyed";
  };
  VkShaderModule shader;
  VULKAN_GUARD(vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shader), "Could not create Shader");
  shader_.set(shader, destroyShader);
  LOG(trace) << "Shader created";
}

} // !namespace vk
} // !namespace yaga
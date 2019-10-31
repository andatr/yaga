#include "precompiled.h"
#include "shader.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Shader::Shader(VkDevice device, asset::Shader* asset)
{
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = asset->Code().Size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(asset->Code().Data());

  auto destroyShader = [device](auto shader) {
    vkDestroyShaderModule(device, shader, nullptr);
    LOG(trace) << "Shader deleted";
  };
  VkShaderModule shader;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shader) != VK_SUCCESS) {
    THROW("Could not create Shader");
  }
  shader_.Assign(shader, destroyShader);
  LOG(trace) << "Shader created";
}

} // !namespace yaga
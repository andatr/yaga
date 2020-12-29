#include "precompiled.h"
#include "shader_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
ShaderPool::ShaderPool(Device* device) :
  device_(device)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ShaderPool::~ShaderPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
VkShaderModule ShaderPool::get(assets::Shader* asset)
{
  auto it = shaders_.find(asset);
  if (it != shaders_.end()) return *it->second;

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = asset->code().size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(asset->code().data());
  auto vkDevice = **device_;
  auto destroyShader = [vkDevice](auto shader) {
    vkDestroyShaderModule(vkDevice, shader, nullptr);
    LOG(trace) << "Shader destroyed";
  };
  VkShaderModule shader_;
  VULKAN_GUARD(vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shader_), "Could not create Shader");
  LOG(trace) << "Shader created";
  AutoDestructor<VkShaderModule> shader(shader_, destroyShader);
  auto ptr = *shader;
  shaders_[asset] = std::move(shader);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
void ShaderPool::clear()
{
  shaders_.clear();
}

} // !namespace vk
} // !namespace yaga
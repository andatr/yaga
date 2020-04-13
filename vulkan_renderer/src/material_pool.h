#ifndef YAGA_VULKAN_RENDERER_SRC_MATERIAL_POOL
#define YAGA_VULKAN_RENDERER_SRC_MATERIAL_POOL

#include <map>
#include <memory>
#include <vector>

#include "device.h"
#include "image_pool.h"
#include "material.h"
#include "swapchain.h"
#include "vulkan.h"
#include "engine/asset/material.h"
#include "engine/asset/shader.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class MaterialPool
{
public:
  explicit MaterialPool(Device* device, Swapchain* swapchain, ImagePool* imagePool, uint32_t maxTextures);
  Material* createMaterial(asset::Material* asset);
  void clear() { materials_.clear(); }
  void swapchain(Swapchain* swapchain);
  const std::vector<VkDescriptorSet>& uniformDescriptorSets() const { return uniformDescriptorSets_; }
private:  
  void createDescriptorLayout();
  void createPipelineLayout();
  void createDescriptorPool(uint32_t maxTextures);
  void createUniformDescriptorSets();
  VkPipeline createPipeline(asset::Shader* vertexShader, asset::Shader* fragmentShader);
  VkShaderModule createShader(asset::Shader* asset);
  std::vector<VkDescriptorSet> createDescriptorSets() const;
  void updateDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets, const std::vector<Image*>& images) const;
private:
  VkDevice vkDevice_;
  Swapchain* swapchain_;
  ImagePool* imagePool_;
  uint32_t frames_;
  AutoDestructor<VkDescriptorSetLayout> uniformLayout_;
  AutoDestructor<VkDescriptorSetLayout> descriptorLayout_;
  AutoDestructor<VkPipelineLayout> pipelineLayout_;
  AutoDestructor<VkDescriptorPool> descriptorPool_;
  std::vector<VkDescriptorSet> uniformDescriptorSets_;
  std::map<asset::Shader*, AutoDestructor<VkShaderModule>> shaders_;
  std::map<std::pair<asset::Shader*, asset::Shader*>, AutoDestructor<VkPipeline>> pipelines_;
  std::map<asset::Material*, MaterialPtr> materials_;
};

typedef std::unique_ptr<MaterialPool> MaterialPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_MATERIAL_POOL

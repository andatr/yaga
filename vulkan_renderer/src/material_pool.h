#ifndef YAGA_VULKAN_RENDERER_SRC_MATERIAL_POOL
#define YAGA_VULKAN_RENDERER_SRC_MATERIAL_POOL

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "device.h"
#include "image_pool.h"
#include "material.h"
#include "swapchain.h"
#include "vulkan.h"
#include "assets/material.h"
#include "assets/shader.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class MaterialPool
{
public:
  explicit MaterialPool(Device* device, Swapchain* swapchain, ImagePool* imagePool, 
    VkDescriptorPool descriptorPool, VkDescriptorSetLayout uniformLayout);
  void swapchain(Swapchain* swapchain);
  MaterialPtr createMaterial(Object* object, assets::Material* asset);
  void removeMaterial(Material* material);
  void clear();
  void updatePipeline(Material* material);

private:
  void createDescriptorLayout();
  void createPipelineLayout();
  AutoDestructor<VkPipeline> createPipeline(assets::Shader* vertexShader, assets::Shader* fragmentShader, bool wireframe);
  VkShaderModule createShader(assets::Shader* asset);
  std::vector<VkDescriptorSet> createDescriptorSets() const;
  void updateDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets, const std::vector<Image*>& images) const;

private:
  struct MaterialCache
  {
    AutoDestructor<VkPipeline> pipeline;
    AutoDestructor<VkPipeline> wireframe;
    std::vector<VkDescriptorSet> descriptorSets;
  };

private:
  VkDevice vkDevice_;
  Swapchain* swapchain_;
  ImagePool* imagePool_;
  VkDescriptorPool descriptorPool_;
  VkDescriptorSetLayout uniformLayout_;
  uint32_t frames_;
  AutoDestructor<VkDescriptorSetLayout> descriptorLayout_;
  AutoDestructor<VkPipelineLayout> pipelineLayout_;
  std::unordered_map<assets::Shader*, AutoDestructor<VkShaderModule>> shaderCache_;
  std::unordered_map<assets::Material*, MaterialCache> materialCache_;
  std::unordered_set<Material*> materials_;
};

typedef std::unique_ptr<MaterialPool> MaterialPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_MATERIAL_POOL

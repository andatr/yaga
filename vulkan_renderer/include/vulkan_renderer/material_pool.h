#ifndef YAGA_VULKAN_RENDERER_MATERIAL_POOL
#define YAGA_VULKAN_RENDERER_MATERIAL_POOL

#include <memory>
#include <unordered_map>

#include "vulkan_renderer/material.h"
#include "vulkan_renderer/shader_pool.h"
#include "vulkan_renderer/texture_pool.h"
#include "vulkan_renderer/vulkan.h"
#include "engine/config.h"
#include "assets/material.h"

namespace yaga {
namespace vk {

class RenderPass3D;

class MaterialPool
{
public:
  MaterialPool(VmaAllocator allocator, RenderPass3D* renderPass, const Config::Resources& config);
  ~MaterialPool();
  MaterialPtr get(assets::MaterialPtr asset);
  void clear();
  void remove(Material* mesh);

private:
  void createSampler();
  void updateDescriptors(Pipeline* pipeline, const std::vector<Image*>& textures) const;

private:
  size_t counter_;
  RenderPass3D* renderPass_;
  ShaderPoolPtr shaderPool_;
  TexturePoolPtr texturePool_;
  AutoDestructor<VkSampler> sampler_;
  std::unordered_map<assets::MaterialPtr, PipelinePtr> materials_;
};

typedef std::unique_ptr<MaterialPool> MaterialPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_MATERIAL_POOL

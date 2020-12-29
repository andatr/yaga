#ifndef YAGA_VULKAN_RENDERER_SRC_MATERIAL_POOL
#define YAGA_VULKAN_RENDERER_SRC_MATERIAL_POOL

#include <memory>
#include <unordered_map>

#include "material.h"
#include "render_pass.h"
#include "shader_pool.h"
#include "swapchain.h"
#include "texture_pool.h"
#include "vulkan.h"
#include "assets/application.h"
#include "assets/material.h"

namespace yaga {
namespace vk {

class MaterialPool
{
public:
  explicit MaterialPool(Swapchain* swapchain, 
    VmaAllocator allocator,
    RenderPass* renderPass,
    const assets::Application* limits);
  ~MaterialPool();
  MaterialPtr get(Object* object, assets::Material* asset);
  void clear();
  void onRemove(Material* mesh);

private:
  PipelinePtr createPipeline(assets::Material* asset) const;
  void createDescriptors(Pipeline* pipeline) const;
  void updateDescriptors(Pipeline* pipeline, const std::vector<Image*>& textures) const;
  //void onResize();

private:
  size_t counter_;
  Swapchain* swapchain_;
  RenderPass* renderPass_;
  ShaderPoolPtr shaderPool_;
  TexturePoolPtr texturePool_;
  std::unordered_map<assets::Material*, PipelinePtr> materials_;
  //Swapchain::Connection resizeConnection_;
};

typedef std::unique_ptr<MaterialPool> MaterialPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_MATERIAL_POOL

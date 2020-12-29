#ifndef YAGA_VULKAN_RENDERER_SRC_CONTEXT
#define YAGA_VULKAN_RENDERER_SRC_CONTEXT

#include <chrono>
#include <memory>

#include "material_pool.h"
#include "mesh_pool.h"
#include "render_pass.h"
#include "renderer_3d_pool.h"
#include "swapchain.h"
#include "vulkan.h"
#include "assets/application.h"
#include "engine/context.h"

namespace yaga {
namespace vk {
  
class Context : public yaga::Context
{
public:
  explicit Context(Swapchain* swapchain,
    VmaAllocator allocator,
    RenderPass* renderPass,
    const assets::Application* limits);
  void update();
  yaga::Renderer3DPtr createRenderer3D(Object* object) override;
  yaga::MaterialPtr   createMaterial  (Object* object, assets::Material* asset) override;
  yaga::MeshPtr       createMesh      (Object* object, assets::Mesh*     asset) override;
  float      delta()      const override { return delta_; }
  glm::uvec2 resolution() const override;
  void clear() override;
  const std::unordered_set<Renderer3D*>& renderers() const { return rendererPool_->all(); }

private:
  typedef std::chrono::high_resolution_clock chrono;

private:
  chrono::time_point prevTime_;
  float             delta_;
  Swapchain*        swapchain_;
  VmaAllocator      allocator_;
  MeshPoolPtr       meshPool_;
  MaterialPoolPtr   materialPool_;
  Renderer3DPoolPtr rendererPool_;
};

typedef std::unique_ptr<Context> ContextPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDERING_CONTEXT

#ifndef YAGA_VULKAN_RENDERER_CONTEXT
#define YAGA_VULKAN_RENDERER_CONTEXT

#include <chrono>
#include <memory>

#include "vulkan_renderer/camera_pool.h"
#include "vulkan_renderer/material_pool.h"
#include "vulkan_renderer/mesh_pool.h"
#include "vulkan_renderer/renderer_3d_pool.h"
#include "vulkan_renderer/post_processor_pool.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"
#include "engine/application.h"
#include "engine/config.h"
#include "engine/context.h"

namespace yaga {
namespace vk {
  
class Context : public yaga::Context
{
public:
  explicit Context(
    Swapchain*      swapchain,
    CameraPool*     cameraPool,
    MeshPool*       meshPool,
    MaterialPool*   materialPool,
    Renderer3DPool* rendererPool);
  void update();
  yaga::Renderer3DPtr createRenderer3D() override;
  yaga::MaterialPtr   createMaterial  (assets::MaterialPtr asset) override;
  yaga::MeshPtr       createMesh      (assets::MeshPtr     asset) override;
  yaga::PostProcessorPtr createPostProcessor(int order, bool hostMemory, PostProcessor::Proc proc) override;
  float      delta()      const override { return delta_; }
  glm::uvec2 resolution() const override;
  void clear() override;
  CameraPtr     createCamera(assets::CameraPtr asset) override;
  yaga::Camera* mainCamera() const override;
  void          mainCamera(yaga::Camera* camera) override;

private:
  typedef std::chrono::high_resolution_clock chrono;

private:
  chrono::time_point   prevTime_;
  float                delta_;
  Swapchain*           swapchain_;
  CameraPool*          cameraPool_;
  MeshPool*            meshPool_;
  MaterialPool*        materialPool_;
  Renderer3DPool*      rendererPool_;
  //PostProcessorPoolPtr processorPool_;
};

typedef std::unique_ptr<Context> ContextPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_RENDERING_CONTEXT

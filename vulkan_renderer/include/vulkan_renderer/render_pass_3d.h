#ifndef YAGA_VULKAN_RENDERER_RENDER_PASS_3D
#define YAGA_VULKAN_RENDERER_RENDER_PASS_3D

#include "vulkan_renderer/camera_pool.h"
#include "vulkan_renderer/material_pool.h"
#include "vulkan_renderer/mesh_pool.h"
#include "vulkan_renderer/renderer_3d_pool.h"
#include "vulkan_renderer/render_layout_3d.h"
#include "vulkan_renderer/render_pass.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"
#include "engine/config.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class RenderPass3D : public RenderPass
{
public:
  RenderPass3D(Swapchain* swapchain, VmaAllocator allocator, ConfigPtr config);
  PipelinePtr createPipeline(VkPipelineShaderStageCreateInfo* shaders, uint32_t count);
  CameraPool*     cameraPool()   { return cameraPool_.get();   }
  MaterialPool*   materialPool() { return materialPool_.get(); }
  MeshPool*       meshPool()     { return meshPool_.get();     }
  Renderer3DPool* rendererPool() { return rendererPool_.get(); }

protected:
  void render(uint32_t imageIndex) override;
  void beginRender(uint32_t imageIndex) override;
  void onResize() override;

private:
  typedef RenderPass base;
  struct Uniform
  {
    BufferPtr buffer;
    VkDescriptorSet descriptor;
  };

private:
  void createDescriptorPool(const Config::Resources& config);
  void createRenderPass();
  void createFrameBuffers();
  void createUniform();
  void createDescriptors(Pipeline* pipeline) const;
  void updateUniform(uint32_t frame);
  void renderObject(uint32_t imageIndex, Renderer3D* object);

private:
  VmaAllocator allocator_;
  RenderLayout3D layout_;
  AutoDestructor<VkDescriptorPool> descriptorPool_;
  std::vector<Uniform> uniform_;
  CameraPoolPtr     cameraPool_;
  MaterialPoolPtr   materialPool_;
  MeshPoolPtr       meshPool_;  
  Renderer3DPoolPtr rendererPool_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_RENDER_STAGE_3D

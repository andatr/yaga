#ifndef YAGA_VULKAN_RENDERER_SRC_RENDERING_CONTEXT
#define YAGA_VULKAN_RENDERER_SRC_RENDERING_CONTEXT

#include <memory>
#include <unordered_set>

#include "buffer.h"
#include "camera_pool.h"
#include "device.h"
#include "image_pool.h"
#include "material_pool.h"
#include "mesh_pool.h"
#include "renderer3d.h"
#include "swapchain.h"
#include "vulkan.h"
#include "assets/application.h"
#include "engine/rendering_context.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class RenderingContext : public yaga::RenderingContext
{
public:
  explicit RenderingContext(Device* device, VmaAllocator allocator, Swapchain* swapchain, const assets::Application* limits);
  void swapchain(Swapchain* swapchain);
  yaga::Renderer3DPtr createRenderer3D(Object* object) override;
  yaga::MaterialPtr createMaterial(Object* object, assets::Material* asset) override;
  yaga::MeshPtr createMesh(Object* object, assets::Mesh* asset) override;
  yaga::CameraPtr createCamera(Object* object, assets::Camera* asset) override;
  const std::unordered_set<Renderer3D*>& renderers3D() const { return renderers3D_; }
  const std::unordered_set<Camera*>& cameras() const { return cameraPool_->cameras(); }
  void removeRenderer3D(Renderer3D* renderer3d);
  void clear() override;
  Camera* mainCamera() const { return cameraPool_->mainCamera(); }
  void mainCamera(Camera* camera) { cameraPool_->mainCamera(camera); }
  glm::uvec2 resolution() override { return resolution_; }
  void update(uint32_t frame);
private:
  void createDescriptorPool(uint32_t maxTextures);
private:
  VkDevice vkDevice_;
  VmaAllocator allocator_;
  uint32_t frames_;
  AutoDestructor<VkDescriptorPool> descriptorPool_;
  CameraPoolPtr cameraPool_;
  ImagePoolPtr imagePool_;
  MaterialPoolPtr materialPool_;
  MeshPoolPtr meshPool_;
  std::unordered_set<Renderer3D*> renderers3D_;
  glm::uvec2 resolution_;
};

typedef std::unique_ptr<RenderingContext> RenderingContextPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RENDERING_CONTEXT

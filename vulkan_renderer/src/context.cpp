#include "precompiled.h"
#include "vulkan_renderer/context.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Context::Context(
  Swapchain*      swapchain,
  CameraPool*     cameraPool,
  MeshPool*       meshPool,
  MaterialPool*   materialPool,
  Renderer3DPool* rendererPool
) :
  prevTime_(chrono::now()),
  delta_(0.0f),
  swapchain_   (swapchain),
  meshPool_    (meshPool),
  cameraPool_  (cameraPool),
  materialPool_(materialPool),
  rendererPool_(rendererPool)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
glm::uvec2 Context::resolution() const
{
  return { 
    swapchain_->resolution().width, 
    swapchain_->resolution().height
  };
}

// -----------------------------------------------------------------------------------------------------------------------------
yaga::MaterialPtr Context::createMaterial(assets::MaterialPtr asset)
{
  return materialPool_->get(asset);
}

// -----------------------------------------------------------------------------------------------------------------------------
yaga::MeshPtr Context::createMesh(assets::MeshPtr asset)
{
  return meshPool_->get(asset);
}

// -----------------------------------------------------------------------------------------------------------------------------
yaga::Renderer3DPtr Context::createRenderer3D()
{
  return rendererPool_->create();
}

// -----------------------------------------------------------------------------------------------------------------------------
CameraPtr Context::createCamera(assets::CameraPtr asset) 
{
  return cameraPool_->get(asset);
}

// -----------------------------------------------------------------------------------------------------------------------------
yaga::Camera* Context::mainCamera() const
{
  return cameraPool_->mainCamera();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Context::mainCamera(yaga::Camera* camera)
{
  auto c = dynamic_cast<Camera*>(camera);
  cameraPool_->mainCamera(c);
}

// -----------------------------------------------------------------------------------------------------------------------------
yaga::PostProcessorPtr Context::createPostProcessor(int, bool, PostProcessor::Proc)
{
  //return processorPool_->create(order, hostMemory, proc);
  return nullptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Context::clear()
{
  swapchain_->device()->waitIdle();
  rendererPool_->clear();
  materialPool_->clear();
  meshPool_->clear();
  //processorPool_->clear();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Context::update()
{
  auto now = chrono::now();
  delta_ = std::chrono::duration<float, std::chrono::seconds::period>(chrono::now() - prevTime_).count();
  prevTime_ = now;
}

} // !namespace vk
} // !namespace yaga
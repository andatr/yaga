#include "precompiled.h"
#include "context.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Context::Context(Swapchain* swapchain,
  VmaAllocator allocator,
  RenderPass* renderPass,
  const assets::Application* limits) :
    prevTime_(chrono::now()),
    delta_(0.0f),
    swapchain_(swapchain)
{
  meshPool_     = std::make_unique<MeshPool>    (swapchain->device(), allocator, limits);
  materialPool_ = std::make_unique<MaterialPool>(swapchain, allocator, renderPass, limits);
  rendererPool_ = std::make_unique<Renderer3DPool>();
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
yaga::MaterialPtr Context::createMaterial(Object* object, assets::Material* asset)
{
  return materialPool_->get(object, asset);
}

// -----------------------------------------------------------------------------------------------------------------------------
yaga::MeshPtr Context::createMesh(Object* object, assets::Mesh* asset)
{
  return meshPool_->get(object, asset);
}

// -----------------------------------------------------------------------------------------------------------------------------
yaga::Renderer3DPtr Context::createRenderer3D(Object* object)
{
  return rendererPool_->create(object);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Context::clear()
{
  swapchain_->device()->waitIdle();
  rendererPool_->clear();
  materialPool_->clear();
  meshPool_->clear();
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
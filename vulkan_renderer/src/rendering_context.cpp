#include "precompiled.h"
#include "rendering_context.h"
#include "material.h"
#include "mesh.h"
#include "renderer3d.h"
#include "shader.h"
#include "assets/vertex.h"

namespace yaga
{
namespace vk
{

// -------------------------------------------------------------------------------------------------------------------------
RenderingContext::RenderingContext(Device* device, VmaAllocator allocator, Swapchain* swapchain,
  const assets::Application* limits) :
    vkDevice_(**device), allocator_(allocator),
    frames_(static_cast<uint32_t>(swapchain->frameBuffers().size()))
{
  resolution_ = { swapchain->resolution().width, swapchain->resolution().height };
  createDescriptorPool(limits->maxTextureCount());
  cameraPool_ = std::make_unique<CameraPool>(device, allocator, swapchain, *descriptorPool_);
  imagePool_ = std::make_unique <ImagePool>(device, allocator, limits->maxImageSize());
  materialPool_ = std::make_unique<MaterialPool>(device, swapchain, imagePool_.get(), *descriptorPool_, cameraPool_->layout());
  meshPool_ = std::make_unique<MeshPool>(device, allocator, limits->maxVertexCount(), limits->maxIndexCount());
}

// -------------------------------------------------------------------------------------------------------------------------
void RenderingContext::swapchain(Swapchain* swapchain)
{
  materialPool_->swapchain(swapchain);
  resolution_ = { swapchain->resolution().width, swapchain->resolution().height };
}

// -------------------------------------------------------------------------------------------------------------------------
yaga::Renderer3DPtr RenderingContext::createRenderer3D(Object* object)
{
  auto renderer3d = std::make_unique<Renderer3D>(object, this);
  renderers3D_.insert(renderer3d.get());
  return renderer3d;
}

// -------------------------------------------------------------------------------------------------------------------------
void RenderingContext::removeRenderer3D(Renderer3D* renderer3d)
{
  vkDeviceWaitIdle(vkDevice_);
  renderers3D_.erase(renderer3d);
}

// -------------------------------------------------------------------------------------------------------------------------
yaga::MaterialPtr RenderingContext::createMaterial(Object* object, assets::Material* asset)
{
  return materialPool_->createMaterial(object, asset);
}

// -------------------------------------------------------------------------------------------------------------------------
yaga::MeshPtr RenderingContext::createMesh(Object* object, assets::Mesh* asset)
{
  return meshPool_->createMesh(object, asset);
}

// -------------------------------------------------------------------------------------------------------------------------
yaga::CameraPtr RenderingContext::createCamera(Object* object, assets::Camera* asset)
{
  return cameraPool_->createCamera(object, asset);
}

// -------------------------------------------------------------------------------------------------------------------------
void RenderingContext::clear()
{
  vkDeviceWaitIdle(vkDevice_);
  cameraPool_->clear();
  materialPool_->clear();
  meshPool_->clear();
}

// ------------------------------------------------------------------------------------------------------------------------
void RenderingContext::createDescriptorPool(uint32_t maxTextures)
{
  std::array<VkDescriptorPoolSize, 2> poolSizes {};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = frames_;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = maxTextures * frames_;

  VkDescriptorPoolCreateInfo poolInfo {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = 10; // TODO: FIX !

  VkDescriptorPool pool;
  auto destroyPool = [device = vkDevice_](auto pool) {
    vkDestroyDescriptorPool(device, pool, nullptr);
    LOG(trace) << "Descriptor Pool destroyed";
  };
  VULKAN_GUARD(vkCreateDescriptorPool(vkDevice_, &poolInfo, nullptr, &pool), "Could not create descriptor pool");
  descriptorPool_.set(pool, destroyPool);
}

// ------------------------------------------------------------------------------------------------------------------------
void RenderingContext::update(uint32_t frame)
{
  cameraPool_->update(frame);
}

} // !namespace vk
} // !namespace yaga
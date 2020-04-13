#include "precompiled.h"
#include "resource_manager.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "shader.h"
#include "engine/vertex.h"

namespace yaga
{
namespace vk
{
namespace
{
} // !namespace

// -------------------------------------------------------------------------------------------------------------------------
ResourceManager::ResourceManager(Device* device, VmaAllocator allocator, Swapchain* swapchain, asset::Application* limits) :
  device_(device), vkDevice_(**device), allocator_(allocator),
  frames_(static_cast<uint32_t>(swapchain->frameBuffers().size()))
{
  imagePool_ = std::make_unique<ImagePool>(device, allocator, limits->maxImageSize());
  materialPool_ = std::make_unique<MaterialPool>(device, swapchain, imagePool_.get(), limits->maxTextureCount());
  meshPool_ = std::make_unique<MeshPool>(device, allocator, limits->maxVertexCount(), limits->maxIndexCount());
  createUniformBuffers();
  updateUniformBuffers(swapchain);
}

// -------------------------------------------------------------------------------------------------------------------------
void ResourceManager::swapchain(Swapchain* swapchain)
{
  materialPool_->swapchain(swapchain);
  updateUniformBuffers(swapchain);
}

// ------------------------------------------------------------------------------------------------------------------------
Scene* ResourceManager::changeScene(asset::Scene* asset)
{
  vkDeviceWaitIdle(vkDevice_);

  objects_.clear();
  meshPool_->clear();
  materialPool_->clear();
  imagePool_->clear();

  const auto modelAsset = asset->model();
  auto mesh = meshPool_->createMesh(modelAsset->mesh());
  auto material = materialPool_->createMaterial(modelAsset->material());

  scene_ = std::make_unique<Scene>();
  auto model = std::make_unique<Model>(mesh, material);
  scene_->root_ = model.get();
  objects_.push_back(std::move(model));
  return scene_.get();
}

// ------------------------------------------------------------------------------------------------------------------------
void ResourceManager::createUniformBuffers()
{
  VkBufferCreateInfo info {};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = sizeof(UniformObject);
  info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo {};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  uniformBuffers_.resize(frames_);
  std::vector<VkWriteDescriptorSet> writers(frames_);
  for (uint32_t i = 0; i < frames_; i++) {
    uniformBuffers_[i] = std::make_unique<Buffer>(allocator_, info, allocInfo);

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = **uniformBuffers_[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformObject);

    writers[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writers[i].dstSet = materialPool_->uniformDescriptorSets()[i];
    writers[i].dstBinding = 0;
    writers[i].dstArrayElement = 0;
    writers[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writers[i].descriptorCount = 1;
    writers[i].pBufferInfo = &bufferInfo;
  }
  vkUpdateDescriptorSets(vkDevice_, frames_, writers.data(), 0, nullptr);
}

// ------------------------------------------------------------------------------------------------------------------------
void ResourceManager::updateUniformBuffers(Swapchain* swapchain)
{
  const auto& size = swapchain->resolution();

  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  UniformObject uniform = {};
  uniform.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 1.0f));
  uniform.view = glm::lookAt(glm::vec3(6.0f, 6.0f, 6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  uniform.projection = glm::perspective(glm::radians(45.0f), (float)size.width / (float)size.height, 0.1f, 100.0f);
  uniform.projection[1][1] *= -1;

  void* mappedData;
  for (uint32_t i = 0; i < frames_; i++) {
    vmaMapMemory(allocator_, uniformBuffers_[i]->allocation(), &mappedData);
    memcpy(mappedData, &uniform, sizeof(UniformObject));
    vmaUnmapMemory(allocator_, uniformBuffers_[i]->allocation());
  }
}

} // !namespace vk
} // !namespace yaga
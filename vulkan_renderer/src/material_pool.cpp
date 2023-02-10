#include "precompiled.h"
#include "vulkan_renderer/material_pool.h"
#include "vulkan_renderer/render_pass_3d.h"
#include "assets/vertex.h"

namespace yaga {
namespace vk {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
VkPipelineShaderStageCreateInfo getShaderStage(VkShaderModule module, VkShaderStageFlagBits shaderType)
{
  VkPipelineShaderStageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = shaderType;
  info.module = module;
  info.pName = "main";
  return info;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPool::MaterialPool(VmaAllocator allocator, RenderPass3D* renderPass, const Config::Resources& config) :
  counter_(0),
  renderPass_(renderPass)
{
  shaderPool_  = std::make_unique<ShaderPool> (renderPass_->swapchain()->device());
  texturePool_ = std::make_unique<TexturePool>(renderPass_->swapchain()->device(), allocator, config.maxImageSize());
  createSampler();
}

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPool::~MaterialPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPtr MaterialPool::get(assets::MaterialPtr asset)
{
  ++counter_;
  auto it = materials_.find(asset);
  if (it != materials_.end()) {
    return std::make_unique<Material>(this, asset, it->second.get());
  }
  std::array<VkPipelineShaderStageCreateInfo, 2> shaders = {
    getShaderStage(shaderPool_->get(asset->vertexShader()),   VK_SHADER_STAGE_VERTEX_BIT),
    getShaderStage(shaderPool_->get(asset->fragmentShader()), VK_SHADER_STAGE_FRAGMENT_BIT)
  };
  auto pipeline = renderPass_->createPipeline(shaders.data(), static_cast<uint32_t>(shaders.size()));
  auto pipelinePtr = pipeline.get();
  std::vector<Image*> textures;
  textures.reserve(asset->textures().size());
  for (const auto& textureAsset : asset->textures()) {
    textures.push_back(texturePool_->get(textureAsset));
  }
  updateDescriptors(pipelinePtr, textures);
  materials_[asset] = std::move(pipeline);
  return std::make_unique<Material>(this, asset, pipelinePtr);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MaterialPool::remove(Material*)
{
  if (counter_ > 0) {
    --counter_;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void MaterialPool::clear()
{
  if (counter_ != 0) {
    THROW("Not all materials were returned to the pool");
  }
  materials_.clear();
  texturePool_->clear();
  shaderPool_->clear();
}

// -----------------------------------------------------------------------------------------------------------------------------
void MaterialPool::updateDescriptors(Pipeline* pipeline, const std::vector<Image*>& textures) const
{
  const auto frames = renderPass_->swapchain()->imageCount();
  std::vector<VkWriteDescriptorSet> writers(textures.size() * frames);
  // TODO: FIX !!!!
  for (size_t i = 0; i < frames; ++i) {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    for (size_t j = 0; j < textures.size(); ++j) {
      auto index = i * textures.size() + j;
      imageInfo.sampler              = *sampler_;
      imageInfo.imageView            = textures[j]->view();
      writers[index].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      writers[index].dstSet          = pipeline->descriptors[i];
      writers[index].dstBinding      = static_cast<uint32_t>(j);
      writers[index].dstArrayElement = 0;
      writers[index].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      writers[index].descriptorCount = 1;
      writers[index].pImageInfo      = &imageInfo;
    }
  }
  const auto device = **renderPass_->swapchain()->device();
  vkUpdateDescriptorSets(device, static_cast<uint32_t>(writers.size()), writers.data(), 0, nullptr);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MaterialPool::createSampler()
{
  VkSamplerCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.unnormalizedCoordinates = VK_FALSE;
  info.magFilter        = VK_FILTER_LINEAR;
  info.minFilter        = VK_FILTER_LINEAR;
  info.addressModeU     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeV     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeW     = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.anisotropyEnable = VK_TRUE;
  info.maxAnisotropy    = 16;
  info.borderColor      = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info.compareEnable    = VK_FALSE;
  info.compareOp        = VK_COMPARE_OP_ALWAYS;
  info.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  info.minLod           = 0;
  info.maxLod           = 0;

  const auto device = **renderPass_->swapchain()->device();
  auto destroySampler = [device](auto sampler) {
    vkDestroySampler(device, sampler, nullptr);
    LOG(trace) << "Texture Sampler destroyed";
  };
  VkSampler sampler{};
  VULKAN_GUARD(vkCreateSampler(device, &info, nullptr, &sampler), "Could not create Texture Sampler");
  sampler_.set(sampler, destroySampler);
}

} // !namespace vk
} // !namespace yaga
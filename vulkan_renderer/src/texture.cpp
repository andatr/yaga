#include "precompiled.h"
#include "vulkan_renderer/texture.h"
#include "vulkan_renderer/texture_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Texture::Texture(
  TexturePool*           pool,
  Device*                device,
  VmaAllocator           allocator,
  VkImageCreateInfo&     info,
  VkImageViewCreateInfo& viewInfo,
  assets::TexturePtr     asset
) :
  dirty_(true),
  pool_(pool),
  Image(device, allocator, info, viewInfo),
  asset_(asset)
{
  connections_.push_back(asset->properties(assets::Image::PropertyIndex::bytes )->onUpdate([this](void*) { dirty_ = true; }));
  connections_.push_back(asset->properties(assets::Image::PropertyIndex::width )->onUpdate([this](void*) { dirty_ = true; }));
  connections_.push_back(asset->properties(assets::Image::PropertyIndex::height)->onUpdate([this](void*) { dirty_ = true; }));
  connections_.push_back(asset->properties(assets::Image::PropertyIndex::format)->onUpdate([this](void*) { dirty_ = true; }));
}

// -----------------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Texture::update()
{
  if (!dirty_) return;
  pool_->update(this);
  dirty_ = false;
}

} // !namespace vk
} // !namespace yaga
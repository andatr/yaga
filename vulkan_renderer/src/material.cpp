#include "precompiled.h"
#include "vulkan_renderer/material.h"
#include "vulkan_renderer/material_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Material::Material(MaterialPool* pool, assets::MaterialPtr asset, Pipeline* pipeline) :
  yaga::Material(asset),
  pool_(pool),
  pipeline_(std::move(pipeline)),
  wireframe_(false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Material::~Material()
{
  pool_->remove(this);
}

} // !namespace vk
} // !namespace yaga
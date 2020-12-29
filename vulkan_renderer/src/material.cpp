#include "precompiled.h"
#include "material.h"
#include "material_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Material::Material(MaterialPool* pool, Object* object, assets::Material* asset, Pipeline* pipeline) :
  yaga::Material(object, asset),
  pool_(pool),
  pipeline_(std::move(pipeline)),
  wireframe_(false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Material::~Material()
{
  pool_->onRemove(this);
}

} // !namespace vk
} // !namespace yaga
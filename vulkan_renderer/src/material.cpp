#include "precompiled.h"
#include "material.h"
#include "material_pool.h"

namespace yaga {
namespace vk {

// -------------------------------------------------------------------------------------------------------------------------
Material::Material(Object* object, assets::Material* asset, MaterialPool* pool, VkPipeline pipeline, 
  VkPipelineLayout layout, const std::vector<VkDescriptorSet>& descriptorSets) :
    yaga::Material(object, asset), pool_(pool), pipeline_(pipeline), layout_(layout), descriptorSets_(descriptorSets)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Material::~Material()
{
  pool_->removeMaterial(this);
}

} // !namespace vk
} // !namespace yaga
#include "precompiled.h"
#include "material.h"

namespace yaga
{
namespace vk
{

// -------------------------------------------------------------------------------------------------------------------------
Material::Material(VkPipeline pipeline, VkPipelineLayout layout, const std::vector<VkDescriptorSet>& descriptors) :
  pipeline_(pipeline), layout_(layout), descriptorSets_(descriptors)
{
}

} // !namespace vk
} // !namespace yaga
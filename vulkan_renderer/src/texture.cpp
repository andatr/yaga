#include "precompiled.h"
#include "texture.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Texture::Texture(Device* device,
  VmaAllocator           allocator,
  VkImageCreateInfo&     info,
  VkImageViewCreateInfo& viewInfo,
  assets::Texture*       asset) :
    Image(device, allocator, info, viewInfo),
    asset_(asset)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
}

} // !namespace vk
} // !namespace yaga
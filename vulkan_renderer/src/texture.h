#ifndef YAGA_VULKAN_RENDERER_SRC_TEXTURE
#define YAGA_VULKAN_RENDERER_SRC_TEXTURE

#include <memory>

#include "image.h"
#include "assets/texture.h"

namespace yaga {
namespace vk {

class Texture : public Image
{
public:
  explicit Texture(Device* device,
    VmaAllocator           allocator,
    VkImageCreateInfo&     info,
    VkImageViewCreateInfo& viewInfo,
    assets::Texture*       asset);
  virtual ~Texture();
  assets::Texture* asset() const { return asset_; }

private:
  assets::Texture* asset_;
};

typedef std::unique_ptr<Texture> TexturePtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_TEXTURE

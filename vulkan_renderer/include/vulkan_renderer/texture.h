#ifndef YAGA_VULKAN_RENDERER_TEXTURE
#define YAGA_VULKAN_RENDERER_TEXTURE

#include <memory>

#include "vulkan_renderer/image.h"
#include "assets/texture.h"

namespace yaga {
namespace vk {

class TexturePool;

class Texture : public Image
{
public:
  explicit Texture(
    TexturePool*           pool,
    Device*                device,
    VmaAllocator           allocator,
    VkImageCreateInfo&     info,
    VkImageViewCreateInfo& viewInfo,
    assets::TexturePtr     asset);
  virtual ~Texture();
  void update();
  assets::TexturePtr asset() const { return asset_; }

private:
  bool dirty_;
  TexturePool* pool_;
  assets::TexturePtr asset_;
  SignalConnections connections_;
};

typedef std::unique_ptr<Texture> TexturePtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_TEXTURE

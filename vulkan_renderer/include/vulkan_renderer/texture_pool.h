#ifndef YAGA_VULKAN_RENDERER_TEXTURE_POOL
#define YAGA_VULKAN_RENDERER_TEXTURE_POOL

#include <map>
#include <memory>

#include "vulkan_renderer/buffer.h"
#include "vulkan_renderer/device.h"
#include "vulkan_renderer/texture.h"
#include "vulkan_renderer/vulkan.h"
#include "assets/texture.h"

namespace yaga {
namespace vk {

class TexturePool
{
public:
  explicit TexturePool(Device* device, VmaAllocator allocator, VkDeviceSize maxSize);
  ~TexturePool();
  Texture* get(assets::TexturePtr asset);
  void clear();
  void update(Texture* texture);

private:
  void createStageBuffer();

private:
  Device*      device_;
  VmaAllocator allocator_;
  VkDeviceSize maxSize_;
  BufferPtr    stageBuffer_;
  std::map<assets::TexturePtr, TexturePtr> textures_;
};

typedef std::unique_ptr<TexturePool> TexturePoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_TEXTURE_POOL

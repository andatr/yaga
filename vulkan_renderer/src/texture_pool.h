#ifndef YAGA_VULKAN_RENDERER_SRC_TEXTURE_POOL
#define YAGA_VULKAN_RENDERER_SRC_TEXTURE_POOL

#include <map>
#include <memory>

#include "buffer.h"
#include "device.h"
#include "texture.h"
#include "vulkan.h"
#include "assets/texture.h"

namespace yaga {
namespace vk {

class TexturePool
{
public:
  explicit TexturePool(Device* device, VmaAllocator allocator, VkDeviceSize maxSize);
  ~TexturePool();
  Texture* get(assets::Texture* asset);
  void clear();

private:
  void createStageBuffer();

private:
  Device*      device_;
  VmaAllocator allocator_;
  VkDeviceSize maxSize_;
  BufferPtr    stageBuffer_;
  std::map<assets::Texture*, TexturePtr> textures_;
};

typedef std::unique_ptr<TexturePool> TexturePoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_TEXTURE_POOL

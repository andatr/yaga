#ifndef YAGA_VULKAN_RENDERER_SRC_IMAGE_POOL
#define YAGA_VULKAN_RENDERER_SRC_IMAGE_POOL

#include <map>
#include <memory>

#include "buffer.h"
#include "device.h"
#include "image.h"
#include "vulkan.h"
#include "assets/texture.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class ImagePool
{
public:
  explicit ImagePool(Device* device, VmaAllocator allocator, VkDeviceSize maxImageSize);
  Image* createImage(assets::Texture* asset);
  void clear() { images_.clear(); }
private:
  void createStageBuffer(VkDeviceSize size);
  void createSampler();
private:
  Device* device_;
  VkDevice vkDevice_;
  VmaAllocator allocator_;
  VkDeviceSize maxImageSize_;
  BufferPtr stageBuffer_;
  AutoDestructor<VkSampler> sampler_;
  std::map<assets::Texture*, ImagePtr> images_;
};

typedef std::unique_ptr<ImagePool> ImagePoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_IMAGE_POOL

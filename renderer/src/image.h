#ifndef YAGA_RENDERER_SRC_IMAGE
#define YAGA_RENDERER_SRC_IMAGE

#include <memory>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device_buffer.h"
#include "asset/texture.h"
#include "utility/auto_destructor.h"

namespace yaga
{

class Image : private boost::noncopyable
{
public:
  explicit Image(VkImage image, VkFormat format);
  explicit Image(Device* device, Allocator* allocator, VkExtent2D size, VkFormat format,
    VkSampleCountFlagBits samples, VkImageUsageFlags usage);
  explicit Image(Device* device, Allocator* allocator, asset::Texture* asset);
  VkFormat format() const { return format_; }
  uint32_t mipLevels() const { return mipLevels_; }
  VkImage operator*() const { return *image_; }
private:
  void createImage(VkExtent2D size, VkImageUsageFlags usage, VkSampleCountFlagBits samples);
  DeviceBufferPtr createBuffer() const;
  void changeLayout(VkImageLayout oldLayout, VkImageLayout newLayout) const;
  void copyBuffer(VkBuffer buffer) const;
  void generateMipMaps();
private:
  Device* device_;
  VkDevice vkDevice_;
  Allocator* allocator_;
  asset::Texture* asset_;
  VkFormat format_;
  uint32_t mipLevels_;
  AutoDestructor<VkDeviceMemory> memory_;
  AutoDestructor<VkImage> image_;
};

typedef std::unique_ptr<Image> ImagePtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_IMAGE

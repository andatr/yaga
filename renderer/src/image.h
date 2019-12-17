#ifndef YAGA_RENDERER_SRC_IMAGE
#define YAGA_RENDERER_SRC_IMAGE

#include <memory>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device_buffer.h"
#include "asset/texture.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class Image : private boost::noncopyable
{
public:
  explicit Image(Device* device, Allocator* allocator, asset::Texture* asset);
  VkImage operator*() const { return *image_; }
private:
  void CreateImage(VkDevice device, Allocator* allocator, asset::Texture* asset);
  DeviceBufferPtr CreateBuffer(VkDevice device, Allocator* allocator, asset::Texture* asset) const;
  void ChangeLayout(Device* device, VkImageLayout oldLayout, VkImageLayout newLayout) const;
  void CopyBuffer(Device* device, VkBuffer buffer, asset::Texture* asset) const;
private:
  AutoDestroyer<VkDeviceMemory> memory_;
  AutoDestroyer<VkImage> image_;
};

typedef std::unique_ptr<Image> ImagePtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_IMAGE

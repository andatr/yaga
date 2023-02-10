#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_FRAME_UPLOADER
#define YAGA_VULKAN_PLAYER_SRC_DECODER_FRAME_UPLOADER

#include <memory>

#include "frame.h"
#include "vulkan_renderer/buffer.h"
#include "vulkan_renderer/swapchain.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class FrameUploader
{
public:
  FrameUploader(Swapchain* swapchain, VmaAllocator allocator);
  bool upload(Frame* frame);
  void changeFormat(FormatPtr format);
  ImageView chroma() { return chromaImage_; }
  ImageView luma()   { return lumaImage_;   }
  FormatPtr format() { return format_;      }

private:
  void createBuffer();
  void createLumaImage();
  void createChromaImage();
  void createCommand();

private:
  Swapchain* swapchain_;
  VmaAllocator allocator_;
  BufferPtr buffer_;
  ImagePtr lumaImage_;
  ImagePtr chromaImage_;
  AutoDestructor<VkCommandBuffer> command_;
  AutoDestructor<VkFence> fence_;
  FormatPtr format_;
};

typedef std::unique_ptr<FrameUploader> FrameUploaderPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_FRAME_UPLOADER

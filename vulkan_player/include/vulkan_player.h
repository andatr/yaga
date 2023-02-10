#ifndef YAGA_VULKAN_PLAYER_VULKAN_PLAYER
#define YAGA_VULKAN_PLAYER_VULKAN_PLAYER

#include <memory>

namespace yaga {

class VideoPlayer
{
public:
  virtual ~VideoPlayer() {}
  virtual void run() = 0;
};

typedef std::unique_ptr<VideoPlayer> VideoPlayerPtr;

namespace vk {

VideoPlayerPtr createVideoPlayer();

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_VULKAN_PLAYER

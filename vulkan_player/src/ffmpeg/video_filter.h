#ifndef YAGA_VULKAN_PLAYER_SRC_FFMPEG_VIDEO_FILTER
#define YAGA_VULKAN_PLAYER_SRC_FFMPEG_VIDEO_FILTER

#include <memory>

#include "ffmpeg.h"

namespace yaga {

class VideoFilter
{
public:
  virtual ~VideoFilter() {}
  virtual void process(AVPacket* packet, uint64_t frame, uint8_t** data, uint64_t* size, int64_t* time);
};

typedef std::unique_ptr<VideoFilter> VideoFilterPtr;

VideoFilterPtr chooseVideoFilter(const char* codecName, AVCodecParameters* codecParams);

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_FFMPEG_VIDEO_FILTER

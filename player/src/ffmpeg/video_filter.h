#ifndef YAGA_PLAYER_SRC_FFMPEG_VIDEO_FILTER
#define YAGA_PLAYER_SRC_FFMPEG_VIDEO_FILTER

#include <memory>

#include "ffmpeg.h"

namespace yaga {
namespace player {

class VideoFilter
{
public:
  virtual ~VideoFilter() {}
  virtual void Demux(AVPacket& packet, uint64_t frame, uint8_t*& data, uint64_t& size, int64_t& time);
};

typedef std::unique_ptr<VideoFilter> VideoFilterPtr;

VideoFilterPtr chooseVideoFilter(AVCodecID codecId, const char* codecName, AVCodecParameters* codecParams);

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_FFMPEG_VIDEO_FILTER

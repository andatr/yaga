#ifndef YAGA_VULKAN_PLAYER_SRC_FFMPEG_MP4_FILTER
#define YAGA_VULKAN_PLAYER_SRC_FFMPEG_MP4_FILTER

#include "video_filter.h"

namespace yaga {

class Mp4Filter : public VideoFilter
{
public:
  Mp4Filter(AVCodecParameters* codecParams);
  ~Mp4Filter();
  void process(AVPacket* packet, uint64_t frame, uint8_t** data, uint64_t* size, int64_t* time) override;

private:
  uint8_t* filteredPacket_;
  uint8_t* extraData_;
  uint64_t extraSize_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_FFMPEG_MP4_FILTER

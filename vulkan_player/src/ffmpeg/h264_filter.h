#ifndef YAGA_VULKAN_PLAYER_SRC_FFMPEG_H264_FILTER
#define YAGA_VULKAN_PLAYER_SRC_FFMPEG_H264_FILTER 

#include "video_filter.h"
#include "utility/auto_destructor.h"

namespace yaga {

class H264Filter : public VideoFilter
{
public:
  H264Filter(const char* codecName, AVCodecParameters* codecParams);
  void process(AVPacket* packet, uint64_t frame, uint8_t** data, uint64_t* size, int64_t* time) override;

private:
  Packet filteredPacket_;
  AutoDestructor<AVBSFContext*> bitstreamFilter_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_FFMPEG_H264_FILTER

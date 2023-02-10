#ifndef YAGA_PLAYER_SRC_FFMPEG_H264_FILTER
#define YAGA_PLAYER_SRC_FFMPEG_H264_FILTER 

#include "video_filter.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace player {

class H264Filter : public VideoFilter
{
public:
  H264Filter(const char* name, AVCodecParameters* codecParams);
  ~H264Filter();
  void Demux(AVPacket& packet, uint64_t frame, uint8_t*& data, uint64_t& size, int64_t& time) override;

private:
  AVPacket filteredPacket_;
  AutoDestructor<AVBSFContext*> bitstreamFilter_;
};

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_FFMPEG_H264_FILTER

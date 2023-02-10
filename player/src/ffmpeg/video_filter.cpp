#include "precompiled.h"
#include "video_filter.h"
#include "h264_filter.h"
#include "mp4_filter.h"

namespace yaga {
namespace player {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
bool isMp4(const char* formatName)
{
  return strcmp(formatName, "QuickTime / MOV")   == 0 ||
         strcmp(formatName, "FLV (Flash Video)") == 0 ||
         strcmp(formatName, "Matroska / WebM")   == 0;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
VideoFilterPtr chooseVideoFilter(AVCodecID codecId, const char* codecName, AVCodecParameters* codecParams)
{
  if (codecId == AV_CODEC_ID_H264 && isMp4(codecName)) {
    return std::make_unique<H264Filter>("h264_mp4toannexb", codecParams);
  }
  else if (codecId == AV_CODEC_ID_HEVC && isMp4(codecName)) {
    return std::make_unique<H264Filter>("hevc_mp4toannexb", codecParams);
  }
  else if (codecId == AV_CODEC_ID_MPEG4 && isMp4(codecName)) {
    return std::make_unique<Mp4Filter>(codecParams);
  }
  return std::make_unique<VideoFilter>();
}

// -----------------------------------------------------------------------------------------------------------------------------
void VideoFilter::Demux(AVPacket& packet, uint64_t, uint8_t*& data, uint64_t& size, int64_t& time)
{
  data = packet.data;
  size = packet.size;
  time = packet.pts;
}

} // !namespace player
} // !namespace yaga

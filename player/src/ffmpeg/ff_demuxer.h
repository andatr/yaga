#ifndef YAGA_PLAYER_SRC_FFMPEG_DEMUXER
#define YAGA_PLAYER_SRC_FFMPEG_DEMUXER

#include <string>
#include <memory>

#include "ffmpeg.h"
#include "demuxer.h"
#include "video_filter.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace player {

class FFDemuxer : public Demuxer
{
public:
  FFDemuxer(const std::string& filename, int64_t timeScaleHz = 1000);
  ~FFDemuxer();
  bool demux(uint8_t*& data, uint64_t& size, int64_t& time) override;
  CodecId codec() override;

private:
  class InitSsl
  {
  public:
    InitSsl();
    ~InitSsl();
  };

private:
  void getPixelFormat();
  void getVideoFilter();
  void getTimeScale();
  void initPacket();

private:
  static InitSsl initSsl_;
  AutoDestructor<AVFormatContext*> context_;
  VideoFilterPtr videoFilter_;
  int videoStream_;
  AVCodecID videoCodec_;
  int width_;
  int height_;
  AVPixelFormat pixelFormat_;
  int bitDepth_;
  int bitsPerPixel_;
  int chromaHeight_;
  double timeBase_;
  int64_t timeScale_;
  AVPacket packet_;
  uint64_t frameCount_;
};

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_FFMPEG_DEMUXER

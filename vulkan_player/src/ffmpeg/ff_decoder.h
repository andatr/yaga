#ifndef YAGA_VULKAN_PLAYER_SRC_FFDECODER
#define YAGA_VULKAN_PLAYER_SRC_FFDECODER

#include "utility/compiler.h"

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include <string>
#include <memory>

#include "ffmpeg.h"
#include "decoder/demuxer.h"
#include "utility/auto_destructor.h"

namespace yaga {

class Codec
{
public:
  Codec(AVFormatContext* formatContext, AVMediaType type);
  bool sendPacket(AVPacket*);
  bool getFrame();

private:
  AutoDestructor<AVCodecContext*> context_;
  int streamIndex_;
  AutoDestructor<AVFrame*> frame_;
};

typedef std::unique_ptr<Codec> CodecPtr;

class FFDecoder
{
public:
  FFDecoder(const std::string& filename);
  ~FFDecoder();
  bool decode();

private:
  bool eof_;
  AutoDestructor<AVFormatContext*> formatContext_;
  std::vector<CodecPtr> codecs_;
  AutoDestructor<AVPacket*> packet_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_FFDECODER

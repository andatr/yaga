#ifndef YAGA_PLAYER_SRC_CODEC
#define YAGA_PLAYER_SRC_CODEC

#include <string>
#include <memory>

namespace yaga {
namespace player {

enum class CodecId
{
  None,
  MPEG1,
  MPEG2,
  MPEG4,
  WMV3,
  VC1,
  H264,
  HEVC,
  VP8,
  VP9,
  MJPEG,
  AV1
};

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_CODEC

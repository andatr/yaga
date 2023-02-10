#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_CODEC
#define YAGA_VULKAN_PLAYER_SRC_DECODER_CODEC

namespace yaga {

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

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_CODEC

#ifndef YAGA_PLAYER_SRC_DECODER
#define YAGA_PLAYER_SRC_DECODER

#include <memory>
#include <vector>

#include "codec.h"
#include "engine/video_player.h"

namespace yaga {
namespace player {

class IDecoder
{
public:
  virtual ~IDecoder() = 0 {}
  virtual void decode(const uint8_t* data, int size, int flags, int64_t timestamp) = 0;
  virtual void getFrame(VideoPlayerFrameProcessor* proc) = 0;
  virtual void resize(uint32_t width, uint32_t height) = 0;
  virtual void stop() = 0;
};

typedef std::unique_ptr<IDecoder> DecoderPtr;

DecoderPtr createDecoder(
  CodecId  codec,
  unsigned clockRate,
  uint32_t width,
  uint32_t height,
  VideoPlayerFrame** frames,
  int frameCount
);

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_DECODER

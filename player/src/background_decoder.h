#ifndef YAGA_PLAYER_SRC_BACKGROUND_DECODER
#define YAGA_PLAYER_SRC_BACKGROUND_DECODER

#include <memory>
#include <string>
#include <thread>

#include "decoder.h"
#include "demuxer.h"

namespace yaga {
namespace player {

class BackgroundDecoder
{
public:
  explicit BackgroundDecoder(
    const std::string& filename,
    uint32_t  width,
    uint32_t  height,
    VideoPlayerFrame** frames,
    int frameCount
  );
  ~BackgroundDecoder();
  void resize(uint32_t width, uint32_t height);
  void get(VideoPlayerFrameProcessor* proc);

private:
  void decode();

private:
  std::atomic_bool running_;
  DemuxerPtr demuxer_;
  DecoderPtr decoder_;
  std::unique_ptr<std::thread> thread_;
};

typedef std::unique_ptr<BackgroundDecoder> BackgroundDecoderPtr;

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_BACKGROUND_DECODER

#include "precompiled.h"
#include "background_decoder.h"

namespace yaga {
namespace player {

// -----------------------------------------------------------------------------------------------------------------------------
BackgroundDecoder::BackgroundDecoder(
  const std::string& filename,
  uint32_t  width,
  uint32_t  height,
  VideoPlayerFrame** frames,
  int frameCount
) :
  running_(true)
{
  demuxer_ = createDemuxer(filename);
  decoder_ = createDecoder(demuxer_->codec(), 1, width, height, frames, frameCount);
  thread_  = std::make_unique<std::thread>([this]() { decode(); });
}

// -----------------------------------------------------------------------------------------------------------------------------
BackgroundDecoder::~BackgroundDecoder()
{
  running_ = false;
  decoder_->stop();
  thread_->join();
}

// -----------------------------------------------------------------------------------------------------------------------------
void BackgroundDecoder::resize(uint32_t width, uint32_t height)
{
  decoder_->resize(width, height);
}

// -----------------------------------------------------------------------------------------------------------------------------
void BackgroundDecoder::get(VideoPlayerFrameProcessor* proc)
{
  decoder_->getFrame(proc);
}

// -----------------------------------------------------------------------------------------------------------------------------
void BackgroundDecoder::decode()
{
  uint8_t* data = nullptr;
  uint64_t size = 0;
  int64_t  time = 0;
  while (running_) {
    if (!demuxer_->demux(data, size, time)) break;
    decoder_->decode(data, size, 0, time);
  }
}

} // !namespace player
} // !namespace yaga
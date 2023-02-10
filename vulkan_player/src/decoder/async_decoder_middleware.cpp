#include "precompiled.h"
#include "async_decoder_middleware.h"

#define FSIZE static_cast<int>(frames_.size())

namespace yaga {
namespace {

const std::vector<Chapter> emptyChapters;
const Description emptyDescription;
const std::string emptyString;

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
AsyncFrame::AsyncFrame() :
  timestamp_(0)
{
}
  
// -----------------------------------------------------------------------------------------------------------------------------
void AsyncFrame::format(const FormatPtr& value)
{
  if (value == format_) return;
  format_ = value;
  if (format_->frameSize > data_.size()) {
    data_.resize(format_->frameSize);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
AsyncDecoderMiddleware::AsyncDecoderMiddleware(uint32_t width, uint32_t height) :
  width_(width),
  heigh_(height),
  running_(false),
  frames_(5),
  timestamp_(0)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
AsyncDecoderMiddleware::~AsyncDecoderMiddleware()
{
  stop();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::resize(uint32_t width, uint32_t height)
{
  width_ = width;
  heigh_ = height;
  resize_ = true;
}

// -----------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::start(const std::string& filename)
{
  if (running_) stop();
  readSync_.count  = 0;
  writeSync_.count = 0;
  resize_ = false;
  running_ = true;
  demuxer_ = createDemuxer(filename, this);
  if (demuxer_->videoStreams()->count() == 0) return;
  auto codec = demuxer_->videoStreams()->stream(demuxer_->videoStreams()->current())->codec();
  auto handler = [this](FrameHandler handler) { this->processFrame(handler); };
  backend_ = createDecoderBackend(codec, 1, width_, heigh_, handler);
  thread_ = std::make_unique<std::thread>([this]() { decode(); });
}

// -----------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::stop()
{
  if (!running_) return;
  running_ = false;
  readSync_.condition.notify_one();
  writeSync_.condition.notify_one();
  thread_->join();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::pause()
{

}

// -----------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::resume()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::seek(int64_t timestamp)
{
  if (demuxer_) return demuxer_->seek(timestamp);
}

// ------------------------------------------------------------------------------------------------------------------------------
int64_t AsyncDecoderMiddleware::duration()
{
  if (demuxer_) return demuxer_->duration();
  return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------
int64_t AsyncDecoderMiddleware::timestamp()
{
  return timestamp_;
}

// ------------------------------------------------------------------------------------------------------------------------------
const Description& AsyncDecoderMiddleware::description()
{
  if (demuxer_) return demuxer_->description();
  return emptyDescription;
}

// ------------------------------------------------------------------------------------------------------------------------------
const std::string& AsyncDecoderMiddleware::title()
{
  if (demuxer_) return demuxer_->title();
  return emptyString;
}

// ------------------------------------------------------------------------------------------------------------------------------
const std::vector<Chapter>& AsyncDecoderMiddleware::chapters()
{
  if (demuxer_) return demuxer_->chapters();
  return emptyChapters;
}

// ------------------------------------------------------------------------------------------------------------------------------
StreamFamily* AsyncDecoderMiddleware::videoStreams()
{
  if (demuxer_) return demuxer_->videoStreams();
  return nullptr;
}

// ------------------------------------------------------------------------------------------------------------------------------
StreamFamily* AsyncDecoderMiddleware::audioStreams()
{
  if (demuxer_) return demuxer_->audioStreams();
  return nullptr;
}

// ------------------------------------------------------------------------------------------------------------------------------
StreamFamily* AsyncDecoderMiddleware::subtitleStreams()
{
  if (demuxer_) return demuxer_->subtitleStreams();
  return nullptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::decode()
{
  while (running_) {
    if (!demuxer_->readFrame()) break;
    if (resize_) {
      resize_ = false;
      backend_->resize(width_, heigh_);
      demuxer_->seek(timestamp_);
    }
  }
}

// ------------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::processVideoFrame(uint8_t* data, uint64_t size, int64_t time)
{
  timestamp_ = time;
  backend_->decode(data, size, 0, time);
}

// ------------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::processAudioFrame(uint8_t*, uint64_t, int64_t)
{
}

// ------------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::processSubtitlesFrame(uint8_t*, uint64_t, int64_t)
{
}

// ------------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::getFrame(Handler handler)
{
  auto frame = popFrameRead();
  handler(frame);
  if (frame) {
    pushFrameRead();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::waitFrame(Handler handler)
{
  auto frame = popFrameRead();
  handler(frame);
  if (frame) {
    pushFrameRead();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::processFrame(FrameHandler handler)
{
  auto frame = popFrameWrite();
  handler(frame);
  pushFrameWrite();
}

// ------------------------------------------------------------------------------------------------------------------------------
Frame* AsyncDecoderMiddleware::popFrameWrite()
{
  std::unique_lock<std::mutex> lock(readSync_.mutex);
  readSync_.condition.wait(lock, [this] { 
    return !running_ || (writeSync_.count >= readSync_.count
      ? writeSync_.count + 1 - readSync_.count != FSIZE
      : readSync_.count != writeSync_.count + 1);
  });
  return &frames_[writeSync_.count];
}

// ------------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::pushFrameWrite()
{
  std::unique_lock<std::mutex> lock(writeSync_.mutex);
  ++writeSync_.count;
  if (writeSync_.count >= FSIZE) {
    writeSync_.count -= FSIZE;
  }
  lock.unlock();
  writeSync_.condition.notify_one();
}

// ------------------------------------------------------------------------------------------------------------------------------
Frame* AsyncDecoderMiddleware::getFrameRead()
{
  std::lock_guard<std::mutex> lock(writeSync_.mutex);
  if (readSync_.count == writeSync_.count) return nullptr;
  return &frames_[readSync_.count];
}

// ------------------------------------------------------------------------------------------------------------------------------
Frame* AsyncDecoderMiddleware::popFrameRead()
{
  std::unique_lock<std::mutex> lock(writeSync_.mutex);
  writeSync_.condition.wait(lock, [this]() {
    return !running_ || (readSync_.count != writeSync_.count);
  });
  return &frames_[readSync_.count];
}

// ------------------------------------------------------------------------------------------------------------------------------
void AsyncDecoderMiddleware::pushFrameRead()
{
  std::unique_lock<std::mutex> lock(readSync_.mutex);
  ++readSync_.count;
  if (readSync_.count >= FSIZE) {
    readSync_.count -= FSIZE;
  }
  lock.unlock();
  readSync_.condition.notify_one();
}

} // !namespace yaga
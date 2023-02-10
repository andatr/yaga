#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_ASYNC_DECODER_MIDDLEWARE
#define YAGA_VULKAN_PLAYER_SRC_DECODER_ASYNC_DECODER_MIDDLEWARE

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "decoder_backend.h"
#include "decoder_middleware.h"
#include "demuxer.h"
#include "player.h"
#include "utility/array.h"

namespace yaga {

class AsyncFrame : public Frame
{
public:
  explicit AsyncFrame();
  virtual ~AsyncFrame() {}
  void timestamp(int64_t value) override { timestamp_ = value; }
  void format(const FormatPtr& value) override;
  int64_t timestamp() const { return timestamp_; }
  const FormatPtr& format() const override { return format_; }
  char* data() override { return *data_; }

private:
  int64_t timestamp_;
  FormatPtr format_;
  ByteArray data_;
};

class AsyncDecoderMiddleware :
  public DecoderMiddleware,
  public DemuxerVisitor,
  public Player
{
public:
  explicit AsyncDecoderMiddleware(uint32_t width, uint32_t height);
  ~AsyncDecoderMiddleware();
  void start(const std::string& filename) override;
  void stop() override;
  void pause() override;
  void resume() override;
  void seek(int64_t timestamp) override;
  int64_t duration()               override;
  int64_t timestamp()              override;
  const Description& description() override;
  const std::string& title()       override;
  const std::vector<Chapter>& chapters() override;
  StreamFamily* videoStreams()    override;
  StreamFamily* audioStreams()    override;
  StreamFamily* subtitleStreams() override;
  void resize(uint32_t width, uint32_t height) override;
  void getFrame(Handler handler) override;
  void waitFrame(Handler handler) override;
  void processVideoFrame    (uint8_t* data, uint64_t size, int64_t time) override;
  void processAudioFrame    (uint8_t* data, uint64_t size, int64_t time) override;
  void processSubtitlesFrame(uint8_t* data, uint64_t size, int64_t time) override;

private:
  struct Sync
  {
    int count;
    std::mutex mutex;
    std::condition_variable condition;
    Sync() : count(0) {}
  };
  typedef std::function<void(Frame* frame)> FrameHandler;

private:
  void decode();
  Frame* popFrameWrite();
  Frame* getFrameRead();
  Frame* popFrameRead();
  void pushFrameWrite();
  void pushFrameRead();
  void processFrame(FrameHandler handler);

private:
  std::atomic_bool running_;
  std::atomic_bool resize_;
  uint32_t width_;
  uint32_t heigh_;
  std::vector<AsyncFrame> frames_;
  DemuxerPtr demuxer_;
  DecoderBackendPtr backend_;  
  std::unique_ptr<std::thread> thread_;
  Sync readSync_;
  Sync writeSync_;
  int64_t timestamp_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_ASYNC_DECODER_MIDDLEWARE

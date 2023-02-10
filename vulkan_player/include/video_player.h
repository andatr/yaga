#ifndef YAGA_ENGINE_VIDEO_PLAYER
#define YAGA_ENGINE_VIDEO_PLAYER

#include <memory>

#include "engine/config.h"

namespace yaga {

class VideoPlayerFrame
{
public:
  virtual ~VideoPlayerFrame() {}
  virtual char*   data()                   = 0;
  virtual size_t  size()                   = 0;
  virtual int64_t timestamp()              = 0;
  virtual void    size(size_t value)       = 0;
  virtual void    timestamp(int64_t value) = 0;
};

class VideoPlayerFrameProcessor
{
public:
  virtual ~VideoPlayerFrameProcessor() {}
  virtual void processFrame(VideoPlayerFrame* frame) = 0;
};

typedef std::unique_ptr<VideoPlayerFrameProcessor> VideoPlayerFrameProcessorPtr;

class VideoPlayerApp
{
public:
  virtual ~VideoPlayerApp() {}
  virtual void init  (uint32_t width, uint32_t height, VideoPlayerFrame** frames, int frameCount) = 0;
  virtual void resize(uint32_t width, uint32_t height) = 0;
  virtual bool loop(VideoPlayerFrameProcessor* proc) = 0;
  virtual void stop()     = 0;
  virtual void shutdown() = 0;
  virtual const Config& config() const = 0;
};

class VideoPlayer
{
public:
  virtual ~VideoPlayer() {}
  virtual void run(VideoPlayerApp* app) = 0;
};

typedef std::unique_ptr<VideoPlayer> VideoPlayerPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_VIDEO_PLAYER

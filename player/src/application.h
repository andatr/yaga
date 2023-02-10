#ifndef YAGA_PLAYER_SRC_APPLICATION
#define YAGA_PLAYER_SRC_APPLICATION

#include <memory>
#include <string>
#include <thread>

#include "background_decoder.h"
#include "engine/config.h"
#include "engine/video_player.h"

namespace yaga {
namespace player {

class Application : public VideoPlayerApp
{
public:
  Application(const Config& config, const std::string& filename);
  virtual ~Application();
  void init  (uint32_t width, uint32_t height, VideoPlayerFrame** frames, int frameCount) override;
  void resize(uint32_t width, uint32_t height) override;
  bool loop(VideoPlayerFrameProcessor* proc) override;
  void stop()     override;
  void shutdown() override;
  const Config& config() const override { return config_; }

private:
  bool running_;
  Config config_;
  std::string filename_;
  BackgroundDecoderPtr decoder_;
};

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_APPLICATION

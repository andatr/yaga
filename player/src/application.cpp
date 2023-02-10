#include "precompiled.h"
#include "application.h"
#include "assets/shader.h"

namespace yaga {
namespace player {

// -----------------------------------------------------------------------------------------------------------------------------
Application::Application(const Config& config, const std::string& filename) :
  running_(false),
  config_(config),
  filename_(filename)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Application::~Application()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::init(uint32_t width, uint32_t height, VideoPlayerFrame** frames, int frameCount)
{
  running_ = true;
  decoder_ = std::make_unique<BackgroundDecoder>(filename_, width, height, frames, frameCount);
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Application::loop(VideoPlayerFrameProcessor* proc)
{
  if (decoder_) {
    decoder_->get(proc);
  }
  return running_;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::resize(uint32_t width, uint32_t height)
{
  if (decoder_) {
    decoder_->resize(width, height);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::stop()
{
  running_ = false;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::shutdown()
{
  decoder_ = nullptr;
}

} // !namespace player
} // !namespace yaga

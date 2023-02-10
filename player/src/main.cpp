#include "precompiled.h"
#include "application.h"
#include "vulkan_player.h"
#include "utility/main.h"

namespace yaga {
namespace player {

// ------------------------------------------------------------------------------------------------------------------------------
void main(const ProgramOptions& options)
{
  auto logSeverity = log::severityFromString(options.config().get<std::string>("log.severity", "info"));
  log::init(logSeverity, log::format::severity | log::format::time);
  auto platform = vk::createVideoPlayer();
  const std::string filename = options.config().get<std::string>("tmp.filename", "");
  auto app = std::make_unique<Application>(Config(options.config()), filename);
  platform->run(app.get());
}

} // !namespace player
} // !namespace yaga

YAGA_MAIN(yaga::player::main)

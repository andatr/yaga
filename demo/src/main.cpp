#include "precompiled.h"
#include "application.h"
#include "utility/main.h"
#include "vulkan_renderer/platform.h"

namespace yaga {
namespace demo {

// ------------------------------------------------------------------------------------------------------------------------------
void main(const ProgramOptions& options)
{
  auto logSeverity = log::severityFromString(options.config().get<std::string>("log.severity", "info"));
  log::init(logSeverity, log::format::severity | log::format::time);
  auto platform = vk::createPlatform(options.config());
  auto app = createApplication(options.config());
  platform->run(app.get());
}

} // !namespace demo
} // !namespace yaga

YAGA_MAIN(yaga::demo::main)
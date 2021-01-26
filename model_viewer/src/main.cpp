#include "precompiled.h"
#include "application.h"
#include "utility/options.h"
#include "vulkan_renderer/platform.h"

namespace yaga {
namespace mview {

// ------------------------------------------------------------------------------------------------------------------------------
void main(const ProgramOptions& options)
{
  auto logSeverity = log::severityFromString(options.config().get<std::string>("log.severity", "info"));
  log::init(logSeverity, log::format::severity | log::format::time);
  auto platform = createPlatform(options.config());
  auto app = createApplication(options.config());
  platform->run(app.get());
}

} // !namespace mview
} // !namespace yaga

// ------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
#ifdef NDEBUG
  try
#endif // !NDEBUG
  {
    yaga::ProgramOptions options(argc, argv);
    yaga::mview::main(options);
  }
#ifdef NDEBUG
  catch (const yaga::Exception& exp) {
    LOG_E(fatal, exp);
    return EXIT_FAILURE;
  } catch (const std::exception& exp) {
    LOG(fatal) << exp.what();
    return EXIT_FAILURE;
  } catch (...) {
    LOG(fatal) << "Unknown exception";
    return EXIT_FAILURE;
  }
#endif // !NDEBUG
  return EXIT_SUCCESS;
}
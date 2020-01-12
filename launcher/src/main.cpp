#include "precompiled.h"
#include "options.h"
#include "version.h"
#include "renderer/application.h"

//#define NDEBUG

namespace yaga
{

// -----------------------------------------------------------------------------------------------------------------------------
void main(const Options& options)
{
  log::init(options.logSeverity(), log::format::Severity | log::format::Time);
  auto app = createApplication();
  app->run(options.appDir());
}

} // !namespace yaga

// -----------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
//fdef NDEBUG
//try
//ndif // !NDEBUG
  {
    yaga::Options options(argc, argv);
    yaga::main(options);
  }
/*ifdef NDEBUG
  catch (const Exception& exp) {
    Log(exp); 
    return EXIT_FAILURE;
  }
  catch (const std::exception& exp) {
    Log(log::Severity::fatal, exp.what());
    return EXIT_FAILURE;
  }
  catch (...) {
    Log(log::Severity::fatal, "Unknown exception");
    return EXIT_FAILURE;
  }
#endif // !NDEBUG*/
  return EXIT_SUCCESS;
}
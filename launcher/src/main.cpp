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
  log::Init(options.LogSeverity());
  auto app = CreateApplication();
  app->Run(options.AppDir());
}



} // !namespace yaga

// -----------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#ifdef NDEBUG
  try
#endif // !NDEBUG
  {
    yaga::Options options(argc, argv);
    yaga::main(options);
  }
#ifdef NDEBUG
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
#endif // !NDEBUG
  return EXIT_SUCCESS;
}
#ifndef YAGA_UTILITY_MAIN
#define YAGA_UTILITY_MAIN

#include "utility/options.h"

#define YAGA_MAIN(mainFunc, OptionDescription)                          \
  int main(int argc, char** argv)                                       \
  {                                                                     \
    return yaga::detail::main<OptionDescription>(mainFunc, argc, argv); \
  }

namespace yaga   {
namespace detail {

// ------------------------------------------------------------------------------------------------------------------------------
template<typename OptionDescription, typename Func>
int main(Func mainFunc, int argc, char** argv)
{
#ifdef NDEBUG
  try {
    const auto options = createOptions<OptionDescription>(argc, argv);
    mainFunc(options);
  }
  catch (const Exception& exp) {
    LOG_E(fatal, exp); 
  } catch (const exception& exp) { 
    LOG(fatal) << exp.what();
  } catch (...) {
    LOG(fatal) << "Unknown exception";
  } 
#else // !NDEBUG
  const auto options = createOptions<OptionDescription>(argc, argv);
  mainFunc(options);
#endif // !NDEBUG
  return EXIT_SUCCESS;
}

} // !namespace detail
} // !namespace yaga

#endif // !YAGA_UTILITY_MAIN
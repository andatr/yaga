#include "precompiled.h"
#include "engine/post_processor.h"

namespace yaga {
namespace {

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
PostProcessor::PostProcessor(int order, bool hostMemory, Proc proc) :
  order_(order),
  host_(hostMemory),
  proc_(proc)
{
}

} // !namespace yaga

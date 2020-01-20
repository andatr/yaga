#include "precompiled.h"
#include "application.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Application::Application() : 
  assets_(std::make_unique<asset::Database>())
{
}
  
// -------------------------------------------------------------------------------------------------------------------------
Application::~Application()
{
}

} // !namespace yaga


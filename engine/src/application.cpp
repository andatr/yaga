#include "precompiled.h"
#include "application.h"

namespace yaga {

// -------------------------------------------------------------------------------------------------------------------------
Application::Application(GamePtr game) : game_(std::move(game))
{
}

// -------------------------------------------------------------------------------------------------------------------------
Application::~Application()
{
}

} // !namespace yaga

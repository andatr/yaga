#ifndef YAGA_VULKAN_RENDERER_APPLICATION
#define YAGA_VULKAN_RENDERER_APPLICATION

#include "engine/application.h"
#include "engine/asset/application.h"

namespace yaga
{

ApplicationPtr createApplication(Game* game, const asset::Application* asset);

} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_APPLICATION

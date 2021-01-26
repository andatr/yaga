#ifndef YAGA_VULKAN_RENDERER_PLATFORM
#define YAGA_VULKAN_RENDERER_PLATFORM

#include <memory>
#include <boost/property_tree/ptree_fwd.hpp>

#include "engine/platform.h"

namespace yaga {

PlatformPtr createPlatform(const boost::property_tree::ptree& options);

} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_PLATFORM

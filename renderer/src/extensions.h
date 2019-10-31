#ifndef YAGA_RENDERER_SRC_EXTENSIONS
#define YAGA_RENDERER_SRC_EXTENSIONS

#include <vector>

#define GET_EXT_PROC_ADDRESS(instance, proc) [](auto inst) { \
  auto result = (PFN_##proc)vkGetInstanceProcAddr(inst, #proc); \
  if (result == nullptr) THROW("Could not get proc address of " #proc); \
  return result; \
} (instance);

namespace yaga
{

const std::vector<const char*> validationLayers = {
#ifndef NDEBUG
"VK_LAYER_KHRONOS_validation"
#endif // !NDEBUG
};

const std::vector<const char*> deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_EXTENSIONS

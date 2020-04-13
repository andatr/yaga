#ifndef YAGA_VULKAN_RENDERER_SRC_VULKAN
#define YAGA_VULKAN_RENDERER_SRC_VULKAN

#include <set>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <vk_mem_alloc/vk_mem_alloc.h>

#define GET_EXT_PROC_ADDRESS(instance, proc) [](auto inst) { \
  auto result = (PFN_##proc)vkGetInstanceProcAddr(inst, #proc); \
  if (result == nullptr) THROW("Could not get proc address of " #proc); \
  return result; \
} (instance);

#define VULKAN_GUARD(func, msg) { \
  if ((func) != VK_SUCCESS) { \
    THROW((msg)); \
  } \
}

#ifndef VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME
#define VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME "VK_KHR_get_memory_requirements2"
#endif

#ifndef VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME
#define VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME "VK_KHR_dedicated_allocation
#endif

#ifndef VK_KHR_BIND_MEMORY_2_EXTENSION_NAME
#define VK_KHR_BIND_MEMORY_2_EXTENSION_NAME "VK_KHR_bind_memory2
#endif

#ifndef VK_EXT_MEMORY_BUDGET_EXTENSION_NAME
#define VK_EXT_MEMORY_BUDGET_EXTENSION_NAME "VK_EXT_memory_budget"
#endif

#ifndef VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME
#define VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME "VK_AMD_device_coherent_memory"
#endif

#ifndef VK_KHR_SWAPCHAIN_EXTENSION_NAME
#define VK_KHR_SWAPCHAIN_EXTENSION_NAME "VK_KHR_swapchain"
#endif

#ifndef VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
#define VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME "VK_KHR_get_physical_device_properties2"
#endif

namespace yaga
{
namespace vk
{

constexpr uint32_t GetVulkanApiVersion()
{
#if VMA_VULKAN_VERSION == 1002000
  return VK_API_VERSION_1_2;
#elif VMA_VULKAN_VERSION == 1001000
  return VK_API_VERSION_1_1;
#elif VMA_VULKAN_VERSION == 1000000
  return VK_API_VERSION_1_0;
#else
#error Invalid VMA_VULKAN_VERSION
  return UINT32_MAX;
#endif
}

const std::vector<const char*> validationLayers = {
#ifndef NDEBUG
  "VK_LAYER_KHRONOS_validation",
  "VK_LAYER_LUNARG_standard_validation"
#endif // !NDEBUG
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_VULKAN

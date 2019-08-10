#ifndef YAGA_PLATFORM_UTILS
#define YAGA_PLATFORM_UTILS

#include <vector>
#include <boost/optional.hpp>

#include "Common/include/Exception.h"

#define GET_EXT_PROC_ADDRESS(instance, proc) [](auto inst) { \
	auto result = (PFN_##proc)vkGetInstanceProcAddr(inst, #proc); \
	if (result == nullptr) THROW("could not get proc address of " #proc) \
	return result; \
} (instance);

namespace yaga
{

#ifdef NDEBUG
	const std::vector<const char*> validationLayers = {}
#else // NDEBUG
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
#endif // !NDEBUG

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLog(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData);

	std::vector<const char*> GetExtensions();

	// contains family indices grouped by features
	struct DeviceFeatures
	{
		std::vector<uint32_t> graphics;
		std::vector<uint32_t> compute;
		std::vector<uint32_t> transfer;
		std::vector<uint32_t> sparceMemory;
		std::vector<uint32_t> protectedMemory;
		std::vector<uint32_t> surface;
	};

	DeviceFeatures GetDeivceFeatures(VkPhysicalDevice device, VkSurfaceKHR surface);
}

#endif // !YAGA_PLATFORM_UTILS
#include "Pch.h"
#include "Utils.h"
#include "Common/include/Log.h"

namespace yaga
{
	// -------------------------------------------------------------------------------------------------------------------------
	std::vector<const char*> GetExtensions()
	{
		uint32_t count = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);
		if (!validationLayers.empty()) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return extensions;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	DeviceFeatures GetDeivceFeatures(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

		std::vector<VkQueueFamilyProperties> families(count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

		DeviceFeatures features = {};
		for (uint32_t i = 0; i < (uint32_t)families.size(); ++i) {
			const auto& family = families[i];
			if (family.queueCount == 0) continue;
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				features.graphics.push_back(i);
			}
			if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
				features.compute.push_back(i);
			}
			if (family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
				features.transfer.push_back(i);
			}
			if (family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
				features.sparceMemory.push_back(i);
			}
			if (family.queueFlags & VK_QUEUE_PROTECTED_BIT) {
				features.protectedMemory.push_back(i);
			}
			VkBool32 surfaceSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surfaceSupport);
			if (surfaceSupport) {
				features.surface.push_back(i);
			}
		}
		return features;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLog(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT /*type*/,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* /*userData*/)
	{
		auto logSeverity = log::Severity::warning;
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			logSeverity = log::Severity::error;
		} 
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			logSeverity = log::Severity::warning;
		}
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			logSeverity = log::Severity::info;
		}
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		{
			logSeverity = log::Severity::debug;
		}
		Log(logSeverity, std::string("validation: ") + callbackData->pMessage);
		return VK_FALSE;
	}
}
#include "Pch.h"
#include "Device.h"
#include "VulkanExtensions.h"
#include "VideoBuffer.h"

namespace yaga
{
namespace
{
	// -------------------------------------------------------------------------------------------------------------------------
	std::vector<std::string> GetDeviceExtension(VkPhysicalDevice device)
	{
		uint32_t count;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
		std::vector<VkExtensionProperties> extensions(count);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());
		
		std::vector<std::string> result;
		std::transform(extensions.begin(), extensions.end(), std::back_inserter(result),
			[] (const auto& prop) { return prop.extensionName; }
		);
		std::sort(result.begin(), result.end());
		return result;
	}	

	// -------------------------------------------------------------------------------------------------------------------------
	Device::DeviceQueues GetDeviceQueues(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

		std::vector<VkQueueFamilyProperties> families(count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

		Device::DeviceQueues queues = {};
		for (uint32_t i = 0; i < (uint32_t)families.size(); ++i) {
			const auto& family = families[i];
			if (family.queueCount == 0) continue;
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				queues.graphics.push_back(i);
			}
			if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
				queues.compute.push_back(i);
			}
			if (family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
				queues.transfer.push_back(i);
			}
			if (family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
				queues.sparceMemory.push_back(i);
			}
			if (family.queueFlags & VK_QUEUE_PROTECTED_BIT) {
				queues.protectedMemory.push_back(i);
			}
			VkBool32 surfaceSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surfaceSupport);
			if (surfaceSupport) {
				queues.surface.push_back(i);
			}
		}
		return queues;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	bool CheckGPUFeatures(VkPhysicalDevice pdevice, const Device::DeviceQueues& queues)
	{
		if (queues.graphics.empty() || queues.surface.empty()) return false;

		auto actualExtensions = GetDeviceExtension(pdevice);
		// need it sorted and unique ...
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		return std::includes(actualExtensions.begin(), actualExtensions.end(), requiredExtensions.begin(), requiredExtensions.end());
	}

} // !namespace

	// -------------------------------------------------------------------------------------------------------------------------
	Device::Device(VkInstance instance, VkSurfaceKHR surface):
		_pdevice(VK_NULL_HANDLE), _graphicsQueue(VK_NULL_HANDLE), _presentQueue(VK_NULL_HANDLE)
	{
		uint32_t count;
		vkEnumeratePhysicalDevices(instance, &count, nullptr);
		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(instance, &count, devices.data());

		// ToDo: ugly, but will do for now
		auto it = std::find_if(devices.begin(), devices.end(), [this, surface](auto device) {
			_queues = GetDeviceQueues(device, surface);
			return CheckGPUFeatures(device, _queues);
		});

		if (it == devices.end()) {
			THROW("could not find Vulkan supporting GPU");
		}

		std::set<uint32_t> familiesSet = { _queues.graphics[0], _queues.surface[0] };
		_families.assign(familiesSet.begin(), familiesSet.end());

		_pdevice = *it;
		CreateDevice(_queues.graphics[0], _queues.surface[0]);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	Device::~Device()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Device::CreateDevice(uint32_t graphicsFamily, uint32_t surfaceFamily)
	{
		std::vector<VkDeviceQueueCreateInfo> queueInfos;

		float priority = 1.0f;
		for (uint32_t family : _families) {
			VkDeviceQueueCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			info.queueFamilyIndex = family;
			info.queueCount = 1;
			info.pQueuePriorities = &priority;
			queueInfos.push_back(info);
		}

		VkPhysicalDeviceFeatures devFeatures = {};
		VkDeviceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		info.pQueueCreateInfos = queueInfos.data();
		info.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
		info.pEnabledFeatures = &devFeatures;
		info.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		info.ppEnabledExtensionNames = deviceExtensions.data();
		if (validationLayers.empty()) {
			info.enabledLayerCount = 0;
		}
		else {
			info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			info.ppEnabledLayerNames = validationLayers.data();
		}

		auto destroyDevice = [](auto device) {
			vkDestroyDevice(device, nullptr);
			LOG(trace) << "Logical Device deleted";
		};
		VkDevice device;
		if (vkCreateDevice(_pdevice, &info, nullptr, &device) != VK_SUCCESS) {
			THROW("Could not create Vulkan device");
		}
		_ldevice.Assign(device, destroyDevice);
		LOG(trace) << "Logical Device created";

		vkGetDeviceQueue(_ldevice.Get(), graphicsFamily, 0, &_graphicsQueue);
		vkGetDeviceQueue(_ldevice.Get(), surfaceFamily, 0, &_presentQueue);
	}
}
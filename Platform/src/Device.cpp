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
	struct DeviceQueues
	{
		std::vector<uint32_t> graphics;
		std::vector<uint32_t> compute;
		std::vector<uint32_t> transfer;
		std::vector<uint32_t> sparceMemory;
		std::vector<uint32_t> protectedMemory;
		std::vector<uint32_t> surface;
	};

	// -------------------------------------------------------------------------------------------------------------------------
	DeviceQueues GetDeviceQueues(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

		std::vector<VkQueueFamilyProperties> families(count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

		DeviceQueues queues = {};
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
	bool CheckGPUFeatures(VkPhysicalDevice pdevice, const DeviceQueues& queues)
	{
		if (queues.graphics.empty() || queues.surface.empty()) return false;

		auto actualExtensions = GetDeviceExtension(pdevice);
		// need it sorted and unique ...
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		return std::includes(actualExtensions.begin(), actualExtensions.end(), requiredExtensions.begin(), requiredExtensions.end());
	}

} // end of anonymous namespace

	// -------------------------------------------------------------------------------------------------------------------------
	Device::Device(VkInstance instance, VkSurfaceKHR surface):
		_pdevice(VK_NULL_HANDLE), _graphicsQueue(VK_NULL_HANDLE), _presentQueue(VK_NULL_HANDLE)
	{
		uint32_t count;
		vkEnumeratePhysicalDevices(instance, &count, nullptr);
		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(instance, &count, devices.data());

		DeviceQueues queues; // ToDo: ugly, but will do for now
		auto it = std::find_if(devices.begin(), devices.end(), [&queues, surface](auto device) {
			queues = GetDeviceQueues(device, surface);
			return CheckGPUFeatures(device, queues);
		});

		if (it == devices.end()) {
			THROW("could not find Vulkan supporting GPU");
		}

		std::set<uint32_t> familiesSet = { queues.graphics[0], queues.surface[0] };
		_families.assign(familiesSet.begin(), familiesSet.end());

		_pdevice = *it;
		CreateDevice(queues.graphics[0], queues.surface[0]);
		_videoBuffer = std::make_unique<VideoBuffer>(_pdevice, _ldevice.Get(), surface, _families);
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

		auto createDevice = [&info, this]() {
			VkDevice device;
			if (vkCreateDevice(_pdevice, &info, nullptr, &device) != VK_SUCCESS) {
				THROW("could not create Vulkan device")
			}
			return device;
		};
		auto destroyDevice = [](auto device) {
			vkDestroyDevice(device, nullptr);
		};
		_ldevice.Construct(createDevice, destroyDevice);
		vkGetDeviceQueue(_ldevice.Get(), graphicsFamily, 0, &_graphicsQueue);
		vkGetDeviceQueue(_ldevice.Get(), surfaceFamily, 0, &_presentQueue);
	}
}
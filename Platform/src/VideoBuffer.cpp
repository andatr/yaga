#include "Pch.h"
#include "VideoBuffer.h"

namespace yaga
{
	namespace
	{
		// -------------------------------------------------------------------------------------------------------------------------
		VkSurfaceFormatKHR PickColorFormat(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			std::vector<VkSurfaceFormatKHR> formats;

			uint32_t count;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
			if (count == 0) {
				THROW("could not choose color format")
			}
			formats.resize(count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, formats.data());
			
			auto format = std::find_if(formats.begin(), formats.end(), [](const auto& f) {
				return f.format == VK_FORMAT_B8G8R8A8_UNORM &&
					f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			});
			return format == formats.end() ? formats[0] : *format;
		}

		// -------------------------------------------------------------------------------------------------------------------------
		VkPresentModeKHR PickBufferingMode(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			std::vector<VkPresentModeKHR> modes;

			uint32_t count;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
			if (count == 0) {
				THROW("could not choose buffering mode")
			}
			modes.resize(count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, modes.data());

			return std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) == modes.end()
				? VK_PRESENT_MODE_FIFO_KHR
				: VK_PRESENT_MODE_MAILBOX_KHR;  // triple buffering, yay
		}

		// -------------------------------------------------------------------------------------------------------------------------
		VkExtent2D PickResolution(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
		{
			// if currentExtent is not MAX_UINT32 we must use values provided by Vulkan
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
				return capabilities.currentExtent;
			}

			// init with min supported values
			VkExtent2D resolution = {
				std::min(capabilities.maxImageExtent.width, width),
				std::min(capabilities.maxImageExtent.height, height)
			};

			// update to max supported
			resolution.width  = std::max(resolution.width,  capabilities.minImageExtent.width);
			resolution.height = std::max(resolution.height, capabilities.minImageExtent.height);
			return resolution;
		}

		// -------------------------------------------------------------------------------------------------------------------------
		uint32_t PickBufferCount(const VkSurfaceCapabilitiesKHR& capabilities)
		{
			if (capabilities.maxImageCount == 0) {
				THROW("could not choose buffering mode")
			}
			return std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VideoBuffer::VideoBuffer(VkPhysicalDevice pdevice, VkDevice ldevice, VkSurfaceKHR surface, const std::vector<uint32_t>& families)
	{
		const auto colorFormat = PickColorFormat(pdevice, surface);
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pdevice, surface, &capabilities);

		VkSwapchainCreateInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.surface = surface;
		info.oldSwapchain = VK_NULL_HANDLE;

		info.minImageCount = PickBufferCount(capabilities);
		info.imageFormat = colorFormat.format;
		info.imageColorSpace = colorFormat.colorSpace;
		info.imageExtent = PickResolution(capabilities, 0, 0);
		info.imageArrayLayers = 1;
		info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (families.size() > 1) {
			info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			info.queueFamilyIndexCount = static_cast<uint32_t>(families.size());
			info.pQueueFamilyIndices = families.data();
		}
		else {
			info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		info.preTransform = capabilities.currentTransform;
		info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		info.presentMode = PickBufferingMode(pdevice, surface);
		info.clipped = VK_TRUE;

		auto createSwapChain = [ldevice, &info, this]() {
			VkSwapchainKHR chain;
			if (vkCreateSwapchainKHR(ldevice, &info, nullptr, &chain) != VK_SUCCESS) {
				THROW("could not create swap chain");
			}
			return chain;
		};
		auto destroSwapChain = [ldevice](auto chain) {
			vkDestroySwapchainKHR(ldevice, chain, nullptr);
		};
		_swapChain.Construct(createSwapChain, destroSwapChain);

		uint32_t imageCount;
		vkGetSwapchainImagesKHR(ldevice, _swapChain.Get(), &imageCount, nullptr);
		_images.resize(imageCount);
		vkGetSwapchainImagesKHR(ldevice, _swapChain.Get(), &imageCount, _images.data());

		//swapChainImageFormat = surfaceFormat.format;
		//swapChainExtent = extent;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VideoBuffer::~VideoBuffer()
	{
	}
}
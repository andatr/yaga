#include "Pch.h"
#include "Texture.h"

namespace yaga
{
	// -------------------------------------------------------------------------------------------------------------------------
	Texture::Texture(VkDevice device, VkImage image, VkFormat format)
	{
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.image = image;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = format;
		info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;

		auto destroyImageView = [device](auto view) {
			vkDestroyImageView(device, view, nullptr);
			LOG(trace) << "Image View deleted";
		};
		VkImageView view;
		if (vkCreateImageView(device, &info, nullptr, &view) != VK_SUCCESS) {
			THROW("Could not create Image View");
		}
		_imageView.Assign(view, destroyImageView);
		LOG(trace) << "Image View created";
	}
}
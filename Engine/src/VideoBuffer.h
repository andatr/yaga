#ifndef YAGA_ENGINE_VIDEO_BUFFER
#define YAGA_ENGINE_VIDEO_BUFFER

#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "AutoDeleter.h"

namespace yaga
{
	class Texture;
	class Device;

	class VideoBuffer : private boost::noncopyable
	{
	public:
		VideoBuffer(Device* device, VkSurfaceKHR surface, VkExtent2D resolution);
		const VkFormat& ImageFormat() const { return _format; }
		const VkExtent2D& Resolution() const { return _resolution; }
		const std::vector<std::unique_ptr<Texture>>& Textures() const { return _textures; }
		VkSwapchainKHR SwapChain() const { return *_swapChain; }
		virtual ~VideoBuffer();
	private:
		VkFormat _format;
		VkExtent2D _resolution;
		AutoDeleter<VkSwapchainKHR> _swapChain;
		std::vector<VkImage> _images;
		std::vector<std::unique_ptr<Texture>> _textures;
	};

	typedef std::unique_ptr<VideoBuffer> VideoBufferPtr;
}

#endif // !YAGA_ENGINE_VIDEO_BUFFER

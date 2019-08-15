#ifndef YAGA_PLATFORM_VIDEO_BUFFER
#define YAGA_PLATFORM_VIDEO_BUFFER

#include <memory>
#include <vector>
#include <set>
#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "Common/include/AutoDeleter.h"

namespace yaga
{
	class VideoBuffer : private boost::noncopyable
	{
	public:
		VideoBuffer(VkPhysicalDevice pdevice, VkDevice ldevice, VkSurfaceKHR surface, const std::vector<uint32_t>& families);
		virtual ~VideoBuffer();
	private:
		AutoDeleter<VkSwapchainKHR> _swapChain;
		std::vector<VkImage> _images;
	};

	typedef std::unique_ptr<VideoBuffer> VideoBufferPtr;
}

#endif // !YAGA_PLATFORM_VIDEO_BUFFER
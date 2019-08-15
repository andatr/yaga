#ifndef YAGA_PLATFORM_DEVICE
#define YAGA_PLATFORM_DEVICE

#include <memory>
#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "Common/include/AutoDeleter.h"

namespace yaga
{
	class VideoBuffer;

	class Device : private boost::noncopyable
	{
	public:
		explicit Device(VkInstance instance, VkSurfaceKHR surface);
		virtual ~Device();
	private:
		void CreateDevice(uint32_t graphicsFamily, uint32_t surfaceFamily);
	private:
		VkPhysicalDevice _pdevice;
		AutoDeleter<VkDevice> _ldevice;
		VkQueue _graphicsQueue;
		VkQueue _presentQueue;
		std::unique_ptr<VideoBuffer> _videoBuffer;
		std::vector<uint32_t> _families;
	};

	typedef std::unique_ptr<Device> DevicePtr;
}

#endif // !YAGA_PLATFORM_DEVICE
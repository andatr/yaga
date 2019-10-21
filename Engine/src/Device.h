#ifndef YAGA_ENGINE_DEVICE
#define YAGA_ENGINE_DEVICE

#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "AutoDeleter.h"

namespace yaga
{
	class VideoBuffer;

	class Device : private boost::noncopyable
	{
	public:
		struct DeviceQueues
		{
			std::vector<uint32_t> graphics;
			std::vector<uint32_t> compute;
			std::vector<uint32_t> transfer;
			std::vector<uint32_t> sparceMemory;
			std::vector<uint32_t> protectedMemory;
			std::vector<uint32_t> surface;
		};
	public:
		explicit Device(VkInstance instance, VkSurfaceKHR surface);
		virtual ~Device();
		VkDevice Logical() const { return *_ldevice; }
		VkPhysicalDevice Physical() const { return _pdevice; }
		const std::vector<uint32_t>& Families() const { return _families; }
		const DeviceQueues& Queues() const { return _queues; }
		VkQueue GraphicsQueue() const { return _graphicsQueue; }
		VkQueue PresentQueue() const { return _presentQueue; }
		uint32_t GetMemoryType(uint32_t filter, VkMemoryPropertyFlags props) const;
	private:
		void CreateDevice(uint32_t graphicsFamily, uint32_t surfaceFamily);
	private:
		VkPhysicalDevice _pdevice;
		AutoDeleter<VkDevice> _ldevice;
		VkQueue _graphicsQueue;
		VkQueue _presentQueue;
		std::vector<uint32_t> _families;
		DeviceQueues _queues;
		VkPhysicalDeviceMemoryProperties _props;
	};

	typedef std::unique_ptr<Device> DevicePtr;
}

#endif // !YAGA_ENGINE_DEVICE

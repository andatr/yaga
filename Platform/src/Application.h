#ifndef YAGA_PLATFORM_APPLICATION
#define YAGA_PLATFORM_APPLICATION

#include <vector>
#include <memory>

#include "include/IApplication.h"
#include "Common/include/SmartDestructor.h"
#include "Utils.h"

namespace yaga
{
	class Application : public IApplication
	{
	public:
		Application(int width, int height, const std::string& title);
		virtual ~Application();
		void Run() override;
	private:
		void CreateWindow();
		void InitVulkan();
		void CreateInstance();
		void CreateSurface();
		void CheckValidationLayers();
		void Loop() const;
		void Cleanup();
		void SetupLogging();
		void FindGPU();
		void CreateDevice(const DeviceFeatures& features);
	private:
		bool _destroyed;
		const int _width;
		const int _height;
		const std::string _title;
		// don't reorder - it's important for destruction
		SmartDestructor<GLFWwindow*> _window;
		SmartDestructor<VkInstance> _instance;
		SmartDestructor<VkDebugUtilsMessengerEXT> _debugMessenger;
		SmartDestructor<VkSurfaceKHR> _surface;
		VkPhysicalDevice _gpu;
		SmartDestructor<VkDevice> _device;
		VkQueue _graphicsQueue;
		VkQueue _presentQueue;
	};
}

#endif // !YAGA_PLATFORM_APPLICATION
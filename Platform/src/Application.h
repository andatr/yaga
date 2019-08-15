#ifndef YAGA_PLATFORM_APPLICATION
#define YAGA_PLATFORM_APPLICATION

#include <memory>
#include <string>
#include <GLFW/glfw3.h>

#include "Common/include/AutoDeleter.h"
#include "include/IApplication.h"

namespace yaga
{
	class Device;
	
	class Application : public IApplication
	{
	public:
		class InitGLFW
		{
		public:
			InitGLFW();
			~InitGLFW();
		};
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
		void SetupLogging();
	private:
		static InitGLFW _initGLFW;
		const int _width;
		const int _height;
		const std::string _title;
		// don't reorder - destruction order is important
		AutoDeleter<GLFWwindow*> _window;
		AutoDeleter<VkInstance> _instance;
		AutoDeleter<VkDebugUtilsMessengerEXT> _debugMessenger;
		AutoDeleter<VkSurfaceKHR> _surface;
		std::unique_ptr<Device> _device;
	};
}

#endif // !YAGA_PLATFORM_APPLICATION
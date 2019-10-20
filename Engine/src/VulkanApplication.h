#ifndef YAGA_ENGINE_VULKAN_APPLICATION
#define YAGA_ENGINE_VULKAN_APPLICATION

#include <memory>
#include <string>
#include <thread>
#include <GLFW/glfw3.h>

#include "AutoDeleter.h"
#include "Application.h"
#include "Material.h"
#include "Asset/Database.h"

namespace yaga
{
	class Device;
	class VideoBuffer;
	class Material;
	namespace asset {
		class Application;
	}

	class VulkanApplication : public Application
	{
	public:
		class InitGLFW
		{
		public:
			InitGLFW();
			~InitGLFW();
		};
	public:
		explicit VulkanApplication();
		virtual ~VulkanApplication();
		void Run(const std::string& dir) override;
	private:
		void CreateWindow(asset::Application* props);
		void CreateInstance(const std::string& appName);
		void CreateVideoBuffer(VkExtent2D resolution);
		void CreateSurface();
		void CreateCommandPool();
		void CheckValidationLayers();
		void Loop();
		void SetupLogging();
		void CreateSync();
		void DrawFrame();
		VkExtent2D GetWindowSize() const;
		static void ResizeCallback(GLFWwindow* window, int width, int height);
	private:
		struct FrameSync
		{
			AutoDeleter<VkSemaphore> render;
			AutoDeleter<VkSemaphore> present;
			AutoDeleter<VkFence> swap;
		};
		struct ResizeInfo
		{
			std::atomic_bool resized;
			VkExtent2D size;
			ResizeInfo() : resized(false), size{} {}
		};
	private:
		static InitGLFW _initGLFW;
		// don't reorder - destruction order is important
		std::unique_ptr<asset::Database> _assets;
		AutoDeleter<GLFWwindow*> _window;
		AutoDeleter<VkInstance> _instance;
		AutoDeleter<VkDebugUtilsMessengerEXT> _debugMessenger;
		AutoDeleter<VkSurfaceKHR> _surface;
		std::unique_ptr<Device> _device;
		std::unique_ptr<VideoBuffer> _videoBuffer;
		std::unique_ptr<Material> _material;
		AutoDeleter<VkCommandPool> _commandPool;
		Array<FrameSync> _frameSync;
		size_t _frame;
		ResizeInfo _resize;
	};
}

#endif // !YAGA_ENGINE_VULKAN_APPLICATION
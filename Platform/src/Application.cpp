#include "Pch.h"
#include "Application.h"
#include "Common/include/Exception.h"

namespace yaga
{
	namespace
	{
		// ---------------------------------------------------------------------------------------------------------------------
		void SetDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info) {
			info = {};
			info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			info.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			info.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			info.pfnUserCallback = VulkanLog;
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	IApplicationPtr CreateApplication(int width, int height, const std::string& title)
	{
		return std::make_unique<Application>(width, height, title);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	Application::Application(int width, int height, const std::string& title):
		_destroyed(false), _width(width), _height(height), _title(title),
		_gpu(VK_NULL_HANDLE), _graphicsQueue(VK_NULL_HANDLE), _presentQueue(VK_NULL_HANDLE)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	Application::~Application()
	{
		Cleanup();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::Cleanup()
	{
		if (_destroyed) return;
		_destroyed = true;
		glfwTerminate();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::Run()
	{
		CreateWindow();
		InitVulkan();
		Loop();
		Cleanup();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::CreateWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		auto createWindow = [this]() {
			auto window = glfwCreateWindow(this->_width, this->_height, this->_title.c_str(), nullptr, nullptr);
			if (!window) {
				THROW("could not create window")
			}
			return window;
		};
		_window.Construct(createWindow, glfwDestroyWindow);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::Loop() const
	{
		while (!glfwWindowShouldClose(_window.Get())) {
			glfwPollEvents();
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::InitVulkan()
	{
		CheckValidationLayers();
		CreateInstance();
		SetupLogging();
		CreateSurface();
		FindGPU();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::CreateInstance()
	{
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = _title.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "yaga";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		if (validationLayers.empty()) {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		} else {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			SetDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}

		auto extensions = GetExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		
		auto createInstance = [&createInfo]() {
			VkInstance instance = {};
			if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
				THROW("could not create instance")
			}
			return instance;
		};
		_instance.Construct(createInstance, [](auto inst){ vkDestroyInstance(inst, nullptr); });
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::CreateSurface()
	{
		auto createSurface = [this]() {
			VkSurfaceKHR surface = {};
			if (glfwCreateWindowSurface(_instance.Get(), _window.Get(), nullptr, &surface) != VK_SUCCESS) {
				THROW("could not create window surface")
			}
			return surface;
		};
		auto destroySurface = [this](auto surface) {
			vkDestroySurfaceKHR(_instance.Get(), surface, nullptr);
		};
		_surface.Construct(createSurface, destroySurface);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::CheckValidationLayers()
	{
		if (validationLayers.empty()) return;

		// get available layers
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// sort available layers
		auto compareLayers = [](const auto& l1, const auto& l2) {
			return strncmp(l1.layerName, l2.layerName, VK_MAX_EXTENSION_NAME_SIZE) < 0;
		};
		std::sort(availableLayers.begin(), availableLayers.end(), compareLayers);

		// check that available layers contain requiered validationLayers
		for (const auto& layer : validationLayers) {
			VkLayerProperties props = {};
			strncpy(props.layerName, layer, VK_MAX_EXTENSION_NAME_SIZE);
			if (!std::binary_search(availableLayers.begin(), availableLayers.end(), props, compareLayers)) {
				THROW("requested validation layer is not available")
			}
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::SetupLogging()
	{
		if (validationLayers.empty()) return;

		VkDebugUtilsMessengerCreateInfoEXT info = {};
		SetDebugMessengerCreateInfo(info);
		auto CreateDebugUtilsMessenger = GET_EXT_PROC_ADDRESS(_instance.Get(), vkCreateDebugUtilsMessengerEXT);
		auto createLogger = [this, CreateDebugUtilsMessenger, info]() {
			VkDebugUtilsMessengerEXT debugMessenger = {};
			if (CreateDebugUtilsMessenger(_instance.Get(), &info, nullptr, &debugMessenger) != VK_SUCCESS) {
				THROW("could not create debug messenger");
			}
			return debugMessenger;
		};

		auto DestroyDebugUtilsMessenger = GET_EXT_PROC_ADDRESS(_instance.Get(), vkDestroyDebugUtilsMessengerEXT);
		auto destroyLogger = [this, DestroyDebugUtilsMessenger](VkDebugUtilsMessengerEXT messenger) {
			DestroyDebugUtilsMessenger(_instance.Get(), messenger, nullptr);
		};

		_debugMessenger.Construct(createLogger, destroyLogger);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::FindGPU()
	{
		uint32_t count = 0;
		vkEnumeratePhysicalDevices(_instance.Get(), &count, nullptr);

		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(_instance.Get(), &count, devices.data());

		for (const auto& device : devices) {
			auto features = GetDeivceFeatures(device, _surface.Get());
			if (features.graphics.empty() || features.surface.empty()) continue;
			_gpu = device;
			CreateDevice(features);
			return;
		}

		THROW("could not find Vulkan supporting GPU");
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::CreateDevice(const DeviceFeatures& features)
	{
		std::vector<VkDeviceQueueCreateInfo> queueInfos;
		const auto graphicsFamily = features.graphics[0];
		const auto surfaceFamily = features.surface[0];
		std::set<uint32_t> families = { graphicsFamily, surfaceFamily };

		float priority = 1.0f;
		for (uint32_t family : families) {
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
		info.enabledExtensionCount = 0;
		if (validationLayers.empty()) {
			info.enabledLayerCount = 0;
		} else {
			info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			info.ppEnabledLayerNames = validationLayers.data();
		}

		auto createDevice = [&info, this]() {
			VkDevice device = {};
			if (vkCreateDevice(_gpu, &info, nullptr, &device) != VK_SUCCESS) {
				THROW("could not create Vulkan device")
			}
			return device;
		};
		auto destroyDevice = [this](auto device) {
			vkDestroyDevice(device, nullptr);
		};
		_device.Construct(createDevice, destroyDevice);
		vkGetDeviceQueue(_device.Get(), graphicsFamily, 0, &_graphicsQueue);
		vkGetDeviceQueue(_device.Get(), surfaceFamily,  0, &_presentQueue );
	}
}
#include "Pch.h"
#include "Application.h"
#include "Device.h"
#include "VulkanExtensions.h"

namespace yaga
{
	Application::InitGLFW Application::_initGLFW;

namespace
{
	// const char* for compatibility with Valukan API
	// -------------------------------------------------------------------------------------------------------------------------
	std::vector<const char*> GetExtensions()
	{
		uint32_t count;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);
		if (!validationLayers.empty()) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		std::sort(extensions.begin(), extensions.end());
		return extensions;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLog(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT /*type*/,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* /*userData*/)
	{
		auto logSeverity = log::Severity::warning;
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			logSeverity = log::Severity::error;
		} 
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			logSeverity = log::Severity::warning;
		}
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			logSeverity = log::Severity::info;
		}
		else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		{
			logSeverity = log::Severity::debug;
		}
		Log(logSeverity, std::string("validation: ") + callbackData->pMessage);
		return VK_FALSE;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void SetDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info)
	{
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

} // end of anonymous namespace

	// -------------------------------------------------------------------------------------------------------------------------
	IApplicationPtr CreateApplication(int width, int height, const std::string& title)
	{
		return std::make_unique<Application>(width, height, title);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	Application::InitGLFW::InitGLFW()
	{
		glfwInit();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	Application::InitGLFW::~InitGLFW()
	{
		glfwTerminate();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	Application::Application(int width, int height, const std::string& title):
		_width(width), _height(height), _title(title)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	Application::~Application()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::Run()
	{
		CreateWindow();
		InitVulkan();
		Loop();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Application::CreateWindow()
	{
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
		_device = std::make_unique<Device>(_instance.Get(), _surface.Get());
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
			VkInstance instance;
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
			VkSurfaceKHR surface;
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
		uint32_t count;
		vkEnumerateInstanceLayerProperties(&count, nullptr);
		std::vector<VkLayerProperties> availableLayers(count);
		vkEnumerateInstanceLayerProperties(&count, availableLayers.data());

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
			VkDebugUtilsMessengerEXT debugMessenger;
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
}
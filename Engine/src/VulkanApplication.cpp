#include "Pch.h"
#include "VulkanApplication.h"
#include "VulkanExtensions.h"
#include "Device.h"
#include "Material.h"
#include "VideoBuffer.h"
#include "Asset/Application.h"
#include "Asset/Material.h"
#include "Asset/Serializer.h"

namespace yaga
{
	VulkanApplication::InitGLFW VulkanApplication::_initGLFW;

namespace
{
	constexpr size_t MAX_FRAMES = 2;

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
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			LOG(error) << "validation: " << callbackData->pMessage;
			return VK_FALSE;
		} 
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			LOG(warning) << "validation: " << callbackData->pMessage;
			return VK_FALSE;
		}
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			LOG(info) << "validation: " << callbackData->pMessage;
			return VK_FALSE;
		}
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
			LOG(debug) << "validation: " << callbackData->pMessage;
			return VK_FALSE;
		}
		LOG(trace) << "validation: " << callbackData->pMessage;
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

} // !namespace

	// -------------------------------------------------------------------------------------------------------------------------
	ApplicationPtr CreateApplication()
	{
		return std::make_unique<VulkanApplication>();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VulkanApplication::InitGLFW::InitGLFW()
	{
		glfwInit();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VulkanApplication::InitGLFW::~InitGLFW()
	{
		glfwTerminate();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VulkanApplication::VulkanApplication() :
		_assets(std::make_unique<asset::Database>()), _frameSync(MAX_FRAMES), _frame(0)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VulkanApplication::~VulkanApplication()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::Run(const std::string& dir)
	{
		asset::Serializer::RegisterStandard();
		asset::Serializer::Deserialize(dir, _assets.get());
		auto props = _assets->Get<asset::Application>("app");
		
		CreateWindow(props);
		CheckValidationLayers();
		CreateInstance(props->Title());
		SetupLogging();
		CreateSurface();
		_device = std::make_unique<Device>(*_instance, *_surface);
		CreateCommandPool();
		CreateVideoBuffer({ props->Width(), props->Height() });

		CreateSync();		
		Loop();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::CreateVideoBuffer(VkExtent2D resolution)
	{
		
		vkDeviceWaitIdle(_device->Logical());
		_model.reset();
		_videoBuffer.reset();			
		_videoBuffer = std::make_unique<VideoBuffer>(_device.get(), *_surface, resolution);

		auto materialAsset = _assets->Get<asset::Material>("material");
		auto material = std::make_unique<Material>(_device.get(), _videoBuffer.get(), *_commandPool, materialAsset);
		auto mesh = std::make_unique<Mesh>(_device.get());
		_model = std::make_unique<Model>(std::move(material), std::move(mesh), _device->Logical(), *_commandPool, _videoBuffer.get());
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::CreateSync()
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		auto deleteSemaphore = [this](auto semaphore) {
			vkDestroySemaphore(_device->Logical(), semaphore, nullptr);
			LOG(trace) << "Vulkan Semaphore deleted";
		};
		auto deleteFence = [this](auto fence) {
			vkDestroyFence(_device->Logical(), fence, nullptr);
			LOG(trace) << "Vulkan Fence deleted";
		};

		auto createSemaphore = [this, &semaphoreInfo](auto& semaphore) {
			if (vkCreateSemaphore(_device->Logical(), &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
				THROW("Could not create Vulkan Semaphore");
			}
			LOG(trace) << "Vulkan Semaphore created";
		};

		VkFence fence;
		VkSemaphore semaphore;		
		for (size_t i = 0; i < _frameSync.Size(); ++i) {
			createSemaphore(semaphore);
			_frameSync[i].render.Assign(semaphore, deleteSemaphore);
			createSemaphore(semaphore);
			_frameSync[i].present.Assign(semaphore, deleteSemaphore);
			if (vkCreateFence(_device->Logical(), &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
				THROW("Could not create Vulkan Fence");
			}
			_frameSync[i].swap.Assign(fence, deleteFence);
			LOG(trace) << "Vulkan Fence created";
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::CreateWindow(asset::Application* props)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		auto deleteWindow = [](auto window) {
			glfwDestroyWindow(window);
			LOG(trace) << "Window deleted";
		};
		auto window = glfwCreateWindow(props->Width(), props->Height(), props->Title().c_str(), nullptr, nullptr);
		if (!window) {
			THROW("Could not create Window");
		}
		_window.Assign(window, deleteWindow);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, ResizeCallback);
		LOG(trace) << "Window created";
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::Loop()
	{
		while (!glfwWindowShouldClose(*_window)) {
			glfwPollEvents();
			DrawFrame();
		}
		vkDeviceWaitIdle(_device->Logical());
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::CreateInstance(const std::string& appName)
	{
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = appName.c_str();
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
		
		auto deleteInstance = [](auto inst){
			vkDestroyInstance(inst, nullptr);
			LOG(trace) << "Vulkan instance deleted";
		};
		VkInstance instance;
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			THROW("Could not create Vulkan Instance");
		}
		_instance.Assign(instance, deleteInstance);
		LOG(trace) << "Vulkan instance created";
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::CreateSurface()
	{
		auto destroySurface = [this](auto surface) {
			vkDestroySurfaceKHR(*_instance, surface, nullptr);
			LOG(trace) << "Surface deleted";
		};
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(*_instance, *_window, nullptr, &surface) != VK_SUCCESS) {
			THROW("Could not create Window Surface");
		}
		_surface.Assign(surface, destroySurface);
		LOG(trace) << "Surface created";
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::CreateCommandPool()
	{
		VkCommandPoolCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.queueFamilyIndex = _device->Queues().graphics[0];

		VkCommandPool commandPool;
		auto destroyCommandPool = [this](auto commandPool) {
			vkDestroyCommandPool(_device->Logical(), commandPool, nullptr);
			LOG(trace) << "Command Pool deleted";
		};
		if (vkCreateCommandPool(_device->Logical(), &info, nullptr, &commandPool) != VK_SUCCESS) {
			THROW("Could not create Command Pool");
		}
		_commandPool.Assign(commandPool, destroyCommandPool);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::CheckValidationLayers()
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
				THROW("requested validation layer is not available");
			}
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::SetupLogging()
	{
		if (validationLayers.empty()) return;

		VkDebugUtilsMessengerCreateInfoEXT info = {};
		SetDebugMessengerCreateInfo(info);
		auto CreateDebugUtilsMessenger = GET_EXT_PROC_ADDRESS(*_instance, vkCreateDebugUtilsMessengerEXT);

		auto DestroyDebugUtilsMessenger = GET_EXT_PROC_ADDRESS(*_instance, vkDestroyDebugUtilsMessengerEXT);
		auto destroyLogger = [this, DestroyDebugUtilsMessenger](VkDebugUtilsMessengerEXT messenger) {
			DestroyDebugUtilsMessenger(*_instance, messenger, nullptr);
			LOG(trace) << "Debug Messenger deleted";
		};

		VkDebugUtilsMessengerEXT debugMessenger;
		if (CreateDebugUtilsMessenger(*_instance, &info, nullptr, &debugMessenger) != VK_SUCCESS) {
			THROW("Could not create Debug Messenger");
		}
		_debugMessenger.Assign(debugMessenger, destroyLogger);
		LOG(trace) << "Debug Messenger created";
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::DrawFrame()
	{
		const auto& device = _device->Logical();
		const auto& sync = _frameSync[_frame];
		const auto swapChain = _videoBuffer->SwapChain();

		vkWaitForFences(device, 1, &*sync.swap, VK_TRUE, UINT64_MAX);

		uint32_t index = 0;
		auto result = vkAcquireNextImageKHR(_device->Logical(), _videoBuffer->SwapChain(),
			UINT64_MAX, *sync.render, VK_NULL_HANDLE, &index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			CreateVideoBuffer(GetWindowSize());
			return;
		}
		if (result != VK_SUCCESS) {
			THROW("Failed to acquire swapchain image");
		}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &*sync.render;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_model->CommandBuffers()[index];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &*sync.present;

		vkResetFences(device, 1, &*sync.swap);
		if (vkQueueSubmit(_device->GraphicsQueue(), 1, &submitInfo, *sync.swap) != VK_SUCCESS) {
			THROW("Could not draw frame");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &*sync.present;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapChain;
		presentInfo.pImageIndices = &index;

		vkQueuePresentKHR(_device->PresentQueue(), &presentInfo);
		_frame = (_frame + 1) % MAX_FRAMES;

		if (_resize.resized) {
			_resize.resized = false;
			CreateVideoBuffer(_resize.size);
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkExtent2D VulkanApplication::GetWindowSize() const
	{
		int width = 0;
		int height = 0;
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(*_window, &width, &height);
			glfwWaitEvents();
		}
		return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void VulkanApplication::ResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<VulkanApplication*>(glfwGetWindowUserPointer(window));
		app->_resize.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
		app->_resize.resized = true;
	}
}
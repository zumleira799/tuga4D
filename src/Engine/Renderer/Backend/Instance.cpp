#include "Instance.h"
#include <Core/Logger.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>

namespace tuga4d::Engine::Renderer::Backend {
	static const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	std::vector<const char*> GetRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (ENABLE_DEBUG_VALIDATION) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
		// Windows requires the win32_surface extension
		extensions.push_back("VK_KHR_win32_surface");
#endif
		return extensions;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		Instance* instance = (Instance*)pUserData;

		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			Logger::Error("[VULKAN] %s", pCallbackData->pMessage);
		} else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			Logger::Warning("[VULKAN] %s", pCallbackData->pMessage);
		} else if (messageSeverity & (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)) {
			Logger::Info("[VULKAN] %s", pCallbackData->pMessage);
		}

		return VK_FALSE;
	}

	static VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pDebugMessenger);
		} else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

    Instance::Instance(int versionMajor, int versionMinor, int versionPatch, const std::string& appName) 
    : vEngineMajor(versionMajor), vEngineMinor(versionMinor), vEnginePatch(versionPatch) {
        VkResult result = volkInitialize();
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to initialise vulkan loader! (volk)");
		}
        CreateInstance(appName);
		if (ENABLE_DEBUG_VALIDATION) {
			CreateDebugMessenger();
		}
    }

    Instance::~Instance(){
		Logger::Trace("Destroying vulkan instance");
		if (ENABLE_DEBUG_VALIDATION) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
        vkDestroyInstance(instance, NULL);
    }

    void Instance::CreateInstance(const std::string& appName) {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_3;

        appInfo.applicationVersion = VK_MAKE_VERSION(vEngineMajor, vEngineMinor, vEnginePatch);
        appInfo.engineVersion = VK_MAKE_VERSION(vEngineMajor, vEngineMinor, vEnginePatch);
        appInfo.pEngineName = "Tuga4D";
        appInfo.pApplicationName = appName.c_str();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

		std::vector instanceExtensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = instanceExtensions.size();
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (ENABLE_DEBUG_VALIDATION) {
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}

        if (VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); result == VK_SUCCESS) {
            Logger::Trace("Created vulkan instance");
        } else {
            throw std::runtime_error("failed to create vulkan instance!");
        }
        volkLoadInstance(instance);
    }
	void Instance::CreateDebugMessenger() {
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
		createInfo.pUserData = this;

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug meslbvnger!");
		}
	}
}
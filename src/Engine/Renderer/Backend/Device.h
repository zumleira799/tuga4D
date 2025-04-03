#pragma once
#include <Core/Util.h>
#include <string>
#include <vulkan/vulkan.h>

namespace tuga4d::Engine::Renderer::Backend {
	class Device : NoCopy, NoMove {
		public:
			Device(Instance& instance, )
		private:
			Instance cInstance;
			VkPhysicalDevice physicalDevice;
			VkDevice device;

			int findQueueFamilies(VkPhysicalDevice phsD, bool* foundAny);
			bool checkDeviceExtensionSupport(VkPhysicalDevice physD, std::vector<char*> checkExt);
			bool isDeviceSuitable(VkPhysicalDevice dev1, std::vector<char*> reqExt);
			void pickPhysicalDevice(VkInstance inst, std::vector<char*> reqExt);
			void createLogicalDevice();
	};
}
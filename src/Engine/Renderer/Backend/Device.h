#pragma once
#include <Core/Util.h>
#include <volk.h>
#include <string>
#include <vector>
#include <vk_mem_alloc.h>

namespace tuga4d::Engine::Renderer::Backend {
    struct SwapchainSupportInfo {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class Instance;
    class DeviceObject;
    class Device : NoCopy, NoMove {
    public:
        Device(Instance& instance, const std::vector<char*>& reqExt);
        ~Device();

        void DestroyObject(DeviceObject* object);

        VkFormat FindSupportedImageFormat(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features);
        VkFormat FindSupportedBufferFormat(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features);

        SwapchainSupportInfo GetSwapchainSupport(VkSurfaceKHR surface);

        VkDevice GetDevice() {
            return device;
        }
        VkPhysicalDevice GetPhysicalDevice() {
            return physicalDevice;
        }
        VmaAllocator GetMemoryAllocator() {
            return memoryAllocator;
        }
        const char* GetDeviceName() {
            return "FIXME";
        }
    private:
        Instance& cInstance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VmaAllocator memoryAllocator = nullptr;
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkQueue graphicsQueue;

        void pickPhysicalDevice(VkInstance inst, const std::vector<char*>& reqExt);
        void createLogicalDevice(const std::vector<char*>& reqExt);
    };
}
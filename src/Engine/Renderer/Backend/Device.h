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
    class CommandPool;
    class DeviceObject;
    class Device : NoCopy, NoMove {
    public:
        Device(Instance& instance, const std::vector<char*>& reqExt);
        ~Device();

        void DestroyObject(DeviceObject* object);

        VkFormat FindSupportedImageFormat(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features);
        VkFormat FindSupportedBufferFormat(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features);

        SwapchainSupportInfo GetSwapchainSupport(VkSurfaceKHR surface);

        CommandPool* RequestCommandPool();
        void ReleaseCommandPool(CommandPool* commandPool);

        VkDevice GetDevice() {
            return device;
        }
        VkPhysicalDevice GetPhysicalDevice() {
            return physicalDevice;
        }
        VkQueue GetQueue() {
            return graphicsQueue;
        }
        uint32_t GetQueueFamilyIndex() {
            return graphicsQueueFamilyIndex;
        }
        VmaAllocator GetMemoryAllocator() {
            return memoryAllocator;
        }
        const char* GetDeviceName() const {
            return deviceProperties.deviceName;
        }
        const VkPhysicalDeviceProperties& GetDeviceProperties() const {
            return deviceProperties;
        }

    private:
        Instance& cInstance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VmaAllocator memoryAllocator = nullptr;
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkPhysicalDeviceProperties deviceProperties{};
        VkQueue graphicsQueue;
        uint32_t graphicsQueueFamilyIndex;

        CommandPool* commandPoolList = nullptr;

        void pickPhysicalDevice(VkInstance inst, const std::vector<char*>& reqExt);
        void createLogicalDevice(const std::vector<char*>& reqExt);
    };
}
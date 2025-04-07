#pragma once
#include <Core/Util.h>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <vma/vk_mem_alloc.h>

namespace tuga4d::Engine::Renderer::Backend {
    class Instance;
    class Device : NoCopy, NoMove {
    public:
        Device(Instance& instance);
        ~Device();

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

        void pickPhysicalDevice(VkInstance inst, std::vector<char*> reqExt);
        void createLogicalDevice();
    };
}
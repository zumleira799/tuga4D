#include "./Instance.h"

namespace tuga4d::Engine::Renderer::Backend{
    int Device::findQueueFamilies(VkPhysicalDevice physD) { 
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(phsD, &queueFamilyCount, NULL);
        VkQueueFamilyProperties* queueFamProp = (VkQueueFamilyProperties*)malloc(queueFamilyCount*sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(phsD, &queueFamilyCount, queueFamProp);
        for(uint32_t i = 0; i < queueFamilyCount; i++){
            if(queueFamProp[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
                free(queueFamProp);
                return (int)i;
            }
        }
    }

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice physD, std::vector<char*> checkExt){
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physD, NULL, &extensionCount, NULL);

    }

    bool Device::isDeviceSuitable(VkPhysicalDevice dev1){
        int index = findQueueFamilies(dev1);

    }

    void Device::pickPhysicalDevice(VkInstance inst){
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(inst, &deviceCount, NULL);
        VkPhysicalDevice* phsDevices = (VkPhysicalDevice*)malloc(deviceCount*sizeof(VkPhysicalDevice));
        vkEnumeratePhysicalDevices(inst, &deviceCount, phsDevices);

    }
}
#include "./Instance.h"
#include <stdexcept>

namespace tuga4d::Engine::Renderer::Backend{
    int Device::findQueueFamilies(VkPhysicalDevice physD, bool* foundAny) { 
        int rV = 0;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(phsD, &queueFamilyCount, NULL);
        VkQueueFamilyProperties* queueFamProp = (VkQueueFamilyProperties*)malloc(queueFamilyCount*sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(phsD, &queueFamilyCount, queueFamProp);
        for(uint32_t i = 0; i < queueFamilyCount; i++){
            if(queueFamProp[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
                free(queueFamProp);
                *foundAny = true;
                return (int)i;
            }
        }
        free(queueFamProp);
        *foundAny = false;
        return rV;
    }

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice physD, std::vector<char*> checkExt){
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physD, NULL, &extensionCount, NULL);
        VkExtensionProperties* dExtProp = (VkExtensionProperties*)malloc(extensionCount*sizeof(VkExtensionProperties));
        vkEnumerateDeviceExtensionProperties(physD, NULL, &extensionCount, dExtProp);

        for(int i = 0; i < checkExt.size(); i++){
            for(int k = 0; k < extensionCount; k++){
                if(strcmp(checkExt[i], dExtProp[k])){
                    goto found;
                }
            }
            return false;
            found:
            continue;
        }
        return true;
    }

    bool Device::isDeviceSuitable(VkPhysicalDevice dev1, std::vector<char*> reqExt){
        bool fnd = false;
        int index = findQueueFamilies(dev1, &fnd);
        bool iS = checkDeviceExtensionSupport(dev1, reqExt);
        //figure out what to do with querying for swapchain support later
        return fnd && iS;
    }

    void Device::pickPhysicalDevice(VkInstance inst, std::vector<char*> reqExt){
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(inst, &deviceCount, NULL);
        if(deviceCount == 0){
            throw std::runtime_error("No GPUs connected\n");
        }
        VkPhysicalDevice* phsDevices = (VkPhysicalDevice*)malloc(deviceCount*sizeof(VkPhysicalDevice));
        vkEnumeratePhysicalDevices(inst, &deviceCount, phsDevices);
        for(int i = 0; i < deviceCount; i++){
            if(isDeviceSuitable(phsDevices[i], reqExt)){
                this.physicalDevice = phsDevices[i];
                return;
            }
        }
        throw std::runtime_error("No suitable GPUs\n");
    }

    void Device::createLogicalDevice(){
        bool t;
        uint32_t index = findQueueFamilies(this.physicalDevice, &t);

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = index;
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};

    }
}
#include "Device.h"
#include "./Instance.h"
#include <Core/Logger.h>
#include <stdexcept>
#include <cassert>

namespace tuga4d::Engine::Renderer::Backend {
    // Returns graphics queue index if found
    static bool findQueueFamilies(VkPhysicalDevice physD, uint32_t* queueIndex) {
        assert(queueIndex != nullptr);
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physD, &queueFamilyCount, NULL);
        VkQueueFamilyProperties* queueFamProp = (VkQueueFamilyProperties*)malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(physD, &queueFamilyCount, queueFamProp);
        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (queueFamProp[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                free(queueFamProp);
                *queueIndex = i;
                return true;
            }
        }
        free(queueFamProp);
        return false;
    }
    static bool checkDeviceExtensionSupport(VkPhysicalDevice physD, const std::vector<char*>& checkExt) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physD, NULL, &extensionCount, NULL);
        VkExtensionProperties* dExtProp = (VkExtensionProperties*)malloc(extensionCount * sizeof(VkExtensionProperties));
        vkEnumerateDeviceExtensionProperties(physD, NULL, &extensionCount, dExtProp);

        for (int i = 0; i < checkExt.size(); i++) {
            for (int k = 0; k < extensionCount; k++) {
                if (strcmp(checkExt[i], dExtProp[k].extensionName)) {
                    goto found;
                }
            }
            free(dExtProp);
            return false;
        found:
            continue;
        }
        free(dExtProp);
        return true;
    }

    static bool isDeviceSuitable(VkPhysicalDevice dev1, const std::vector<char*>&reqExt) {
        uint32_t index;
        bool fnd = findQueueFamilies(dev1, &index);
        bool iS = checkDeviceExtensionSupport(dev1, reqExt);
        //figure out what to do with querying for swapchain support later
        return fnd && iS;
    }

    Device::Device(Instance& instance) : cInstance(instance) {

        pickPhysicalDevice(instance.GetInstance(), /*required extensions*/);
        createLogicalDevice(/*required extensions*/);
        Logger::Trace("Created device %s", GetDeviceName());
    }

    Device::~Device() {
        vkDestroyDevice(device, VK_NULL_HANDLE);
    }


    void Device::pickPhysicalDevice(VkInstance inst, const std::vector<char*>&reqExt) {
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(inst, &deviceCount, NULL);
        if (deviceCount == 0) {
            throw std::runtime_error("No GPUs connected\n");
        }
        VkPhysicalDevice* phsDevices = (VkPhysicalDevice*)malloc(deviceCount * sizeof(VkPhysicalDevice));
        vkEnumeratePhysicalDevices(inst, &deviceCount, phsDevices);
        for (int i = 0; i < deviceCount; i++) {
            if (isDeviceSuitable(phsDevices[i], reqExt)) {
                this->physicalDevice = phsDevices[i];
                free(phsDevices);
                return;
            }
        }
        free(phsDevices);
        throw std::runtime_error("No suitable GPUs\n");
    }

    void Device::createLogicalDevice(const std::vector<char*>&reqExt) {
        uint32_t index;
        findQueueFamilies(physicalDevice, &index);

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = index;
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;

        deviceCreateInfo.enabledExtensionCount = reqExt.size();
        deviceCreateInfo.ppEnabledExtensionNames = reqExt.data();
        deviceCreateInfo.pEnabledFeatures = deviceFeatures;

        vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);

        volkLoadDevice(device);

        vkGetDeviceQueue(device, index, 0, &graphicsQueue);
    }
}
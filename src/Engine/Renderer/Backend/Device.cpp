#include "Device.h"
#include "./DeviceObject.h"
#include "./Instance.h"
#include <Core/Logger.h>
#include <Engine/Renderer/Backend/CommandPool.h>
#include <stdexcept>
#include <cassert>
#include <cstring>

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
    /*
        MSAA,
        Anisotropic filtering
        Wireframe
        Independent alpha blend
        dynamic rendering
    */
    static bool checkDeviceFeatureSupport(VkPhysicalDevice physD, const VkPhysicalDeviceFeatures& feats) {
        VkPhysicalDeviceDynamicRenderingFeatures dynRenderingFeats{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };
        VkPhysicalDeviceFeatures2 feats2;
        feats2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        feats2.pNext = &dynRenderingFeats;
        vkGetPhysicalDeviceFeatures2(physD, &feats2);
        return dynRenderingFeats.dynamicRendering && feats.sampleRateShading
            && feats.fillModeNonSolid && feats.samplerAnisotropy && feats.wideLines && feats.independentBlend;
    }
    static void enableDeviceFeatures(VkPhysicalDeviceFeatures* feats) {
        feats->sampleRateShading = true;
        feats->fillModeNonSolid = true;
        feats->samplerAnisotropy = true;
        feats->wideLines = true;
        feats->independentBlend = true;
    }


    static bool isDeviceSuitable(VkPhysicalDevice dev1, const std::vector<char*>&reqExt, const VkPhysicalDeviceFeatures& features) {
        uint32_t index;
        bool fnd = findQueueFamilies(dev1, &index);
        bool iS = checkDeviceExtensionSupport(dev1, reqExt);
        bool iF = checkDeviceFeatureSupport(dev1, features);
        //figure out what to do with querying for swapchain support later
        return fnd && iF&& iS;
    }

    Device::Device(Instance& instance, const std::vector<char*>&reqExt) : cInstance(instance) {
        pickPhysicalDevice(instance.GetInstance(), reqExt);
        createLogicalDevice(reqExt);
        Logger::Trace("Created device %s", GetDeviceName());
        Logger::Trace("Using extensions: ");
        for (char* str : reqExt) {
            printf("\t%s\n", str);
        }
    }

    Device::~Device() {
        Logger::Trace("Destroying device %s", GetDeviceName());
        CommandPool* cp = commandPoolList;
        while(cp != NULL){
            CommandPool* cp1 = cp->next;
            delete cp;
            cp = cp1;
        }
        vkDestroyDevice(device, VK_NULL_HANDLE);
    }
    void Device::DestroyObject(DeviceObject* object) {
        // Here you should queue this to be destroyed layer when it's safe
        vkDeviceWaitIdle(device); // todo: remove
        delete object;
    }
    VkFormat Device::FindSupportedImageFormat(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
            if ((props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported image format!");
    }
    VkFormat Device::FindSupportedBufferFormat(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
            if ((props.bufferFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported buffer format!");
    }

    SwapchainSupportInfo Device::GetSwapchainSupport(VkSurfaceKHR surface) {
        SwapchainSupportInfo support{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &support.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            support.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, support.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            support.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, support.presentModes.data());
        }
        return support;
    }

    CommandPool* Device::RequestCommandPool() {
        CommandPool* cp = commandPoolList;
        if(cp == nullptr){
            cp = new CommandPool(*this, "");
        }
        commandPoolList = cp->next;
        return cp;
    }

    void Device::ReleaseCommandPool(CommandPool* commandPool) {
        assert(commandPool != nullptr);
        commandPool->next = commandPoolList; 
        commandPoolList = commandPool;
    }

    void Device::pickPhysicalDevice(VkInstance inst, const std::vector<char*>&reqExt) {
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(inst, &deviceCount, NULL);
        if (deviceCount == 0) {
            throw std::runtime_error("No GPUs connected\n");
        }
        VkPhysicalDevice* phsDevices = (VkPhysicalDevice*)malloc(deviceCount * sizeof(VkPhysicalDevice));
        vkEnumeratePhysicalDevices(inst, &deviceCount, phsDevices);
        Logger::Trace("Devices available: ");
        for (int i = 0; i < deviceCount; ++i) {
            vkGetPhysicalDeviceProperties(phsDevices[i], &deviceProperties);
            printf("\t%s\n", deviceProperties.deviceName);
        }
        for (int i = 0; i < deviceCount; i++) {
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceFeatures(phsDevices[i], &deviceFeatures);
            vkGetPhysicalDeviceProperties(phsDevices[i], &deviceProperties);
            //bool validGPU = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
            //    deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            if (isDeviceSuitable(phsDevices[i], reqExt, deviceFeatures) /* && validGPU*/) {
                this->physicalDevice = phsDevices[i];
                enableDeviceFeatures(&enabledDeviceFeatures);
                free(phsDevices);
                return;
            }
        }
        free(phsDevices);
        throw std::runtime_error("No suitable GPUs\n");
    }

    void Device::createLogicalDevice(const std::vector<char*>&reqExt) {
        findQueueFamilies(physicalDevice, &graphicsQueueFamilyIndex);
        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;

        deviceCreateInfo.enabledExtensionCount = reqExt.size();
        deviceCreateInfo.ppEnabledExtensionNames = reqExt.data();
        deviceCreateInfo.pEnabledFeatures = &enabledDeviceFeatures;

        dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
        VkPhysicalDeviceFeatures2 feats2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        feats2.pNext = &dynamicRenderingFeatures;
        deviceCreateInfo.pNext = &feats2;

        vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);

        volkLoadDevice(device);

        vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    }
}
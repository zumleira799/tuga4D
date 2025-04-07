#include "Instance.h"
#include <Core/Logger.h>
#include <stdexcept>


namespace tuga4d::Engine::Renderer::Backend {
    Instance::Instance(int versionMajor, int versionMinor, int versionPatch, const std::string& appName) 
    : vEngineMajor(versionMajor), vEngineMinor(versionMinor), vEnginePatch(versionPatch) {
        volkInitialize();
        CreateInstance(appName);
    }

    Instance::~Instance(){
        vkDestroyInstance(instance, NULL);
    }

    void Instance::CreateInstance(const std::string& appName) {

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

        appInfo.applicationVersion = VK_MAKE_VERSION(vEngineMajor, vEngineMinor, vEnginePatch);
        appInfo.engineVersion = VK_MAKE_VERSION(vEngineMajor, vEngineMinor, vEnginePatch);
        appInfo.pEngineName = "Tuga4D";
        appInfo.pApplicationName = appName.c_str();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        
        // FIXME: add required extensions to the instance!

        if (VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); result == VK_SUCCESS) {
            Logger::Trace("Created vulkan instance");
        } else {
            throw std::runtime_error("failed to create vulkan instance!");
        }
        volkLoadInstance(instance);
    }
}
#pragma once
#include <Core/Util.h>
#include <vulkan/vulkan.h>
#include <string>

namespace tuga4d::Engine::Renderer::Backend {
    class Device;
    class DeviceObject : NoCopy, NoMove {
    public:
        DeviceObject() = default;
        ~DeviceObject();
        const char* GetDebugName() {
            return debugName.empty() ? "Missing Debug Name" : debugName.c_str();
        }

        // returns if the resource is valid
        virtual bool IsOk() const = 0;
    protected:
        virtual void OnDestruct() = 0;
        void CreateDebugInfo(Device& device, const std::string& debugName,
            uint64_t object, VkDebugReportObjectTypeEXT objectType);
    private:
        std::string debugName;
    };
}
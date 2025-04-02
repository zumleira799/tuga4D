#pragma once
#include <Core/Util.h>
#include <vulkan/vulkan.h>

namespace tuga4d::Engine::Renderer::Backend {
    class Device;
    class DeviceObject : NoCopy, NoMove {
    public:
        DeviceObject() = default;
        virtual ~DeviceObject() {}
        const char* GetDebugName() {
            return debugName ? "Missing Debug Name" : debugName;
        }

        // returns if the resource is valid
        virtual bool IsOk() const = 0;
    protected:
        void CreateDebugInfo(Device& device, const char* debugName,
            uint64_t object, VkDebugReportObjectTypeEXT objectType);
    private:
        const char* debugName;
    };
}
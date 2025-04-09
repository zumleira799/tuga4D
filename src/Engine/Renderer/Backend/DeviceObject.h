#pragma once
#include <Core/Util.h>
#include <volk.h>
#include <string>

namespace tuga4d::Engine::Renderer::Backend {
    class Device;
    class DeviceObject : NoCopy, NoMove {
    public:
        DeviceObject(Device& device);

        const char* GetDebugName() {
            return debugName.empty() ? "Missing Debug Name" : debugName.c_str();
        }

        // returns if the resource is valid
        virtual bool IsOk() const = 0;
    protected:
        void CreateDebugInfo(const std::string& debugName,
            uint64_t object, VkDebugReportObjectTypeEXT objectType);

        friend class Device; // Only device class is allowed to call the destructor
        virtual ~DeviceObject();
        Device& device;
    private:
        std::string debugName;
    };
}
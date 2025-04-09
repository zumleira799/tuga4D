#include "DeviceObject.h"
#include <Engine/Renderer/Backend/Instance.h>
#include <Engine/Renderer/Backend/Device.h>
#include <Core/Logger.h>
#include <typeinfo>

namespace tuga4d::Engine::Renderer::Backend{
    DeviceObject::DeviceObject(Device& device) : device(device) {

    }

    DeviceObject::~DeviceObject() {
        Logger::Trace("Destroying object %s", GetDebugName());
    }
    void DeviceObject::CreateDebugInfo(const std::string& debugName,
        uint64_t object, VkDebugReportObjectTypeEXT objectType) {
        this->debugName = debugName;
        if (!ENABLE_DEBUG_VALIDATION) return;
        if (!debugName.empty()) {
            VkDebugMarkerObjectNameInfoEXT objectInfo{ VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT };
            objectInfo.object = object;
            objectInfo.objectType = objectType;
            objectInfo.pObjectName = debugName.c_str();
            objectInfo.pNext = nullptr;
            if (VkResult result = vkDebugMarkerSetObjectNameEXT(device.GetDevice(), &objectInfo); result != VK_SUCCESS) {
                Logger::Warning("Failed to set vulkan object debug name! VkResult: %i", static_cast<int>(result));
            }
            Logger::Trace("Initialised device object %s %s", typeid(this).name(), debugName);
        } else {
            Logger::Trace("Initialised device object %s %p", typeid(this).name(), this);
        }
    }
}

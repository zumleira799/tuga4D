#pragma once
#include <Engine/Renderer/Backend/DeviceObject.h>

namespace tuga4d::Engine::Renderer::Backend {
    class CommandPool;
    class CommandBuffer : public DeviceObject {
    public:
        CommandBuffer(Device& device, const std::string& debugName);

        VkCommandBuffer GetCommandBuffer() {
            return commandBuffer;
        }

        bool IsOk() const {
            return commandBuffer != VK_NULL_HANDLE;
        }
    protected:
        virtual ~CommandBuffer();
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        CommandPool* owningCommandPool = nullptr;
    };
}
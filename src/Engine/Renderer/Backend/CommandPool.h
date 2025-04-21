#pragma once
#include <Engine/Renderer/Backend/DeviceObject.h>

namespace tuga4d::Engine::Renderer::Backend {
    class CommandPool : public DeviceObject {
    public:
        CommandPool(Device& device, const std::string& debugName);

        VkCommandPool GetCommandPool() {
            return commandPool;
        }

        bool IsOk() const {
            return commandPool != VK_NULL_HANDLE;
        }
    protected:
        virtual ~CommandPool();
    private:
        VkCommandPool commandPool = VK_NULL_HANDLE;

        friend class Device; // device can manage the command pools
        CommandPool* next = nullptr;
    };
}
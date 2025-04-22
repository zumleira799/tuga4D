#include "CommandPool.h"
#include <Engine/Renderer/Backend/Device.h>
#include <stdexcept>
namespace tuga4d::Engine::Renderer::Backend {
    CommandPool::CommandPool(Device& device, const std::string& debugName) : DeviceObject(device) {
        VkCommandPoolCreateInfo createInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        createInfo.queueFamilyIndex = device.GetQueueFamilyIndex();
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        if (vkCreateCommandPool(device.GetDevice(), &createInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
        CreateDebugInfo(debugName, reinterpret_cast<uint64_t>(commandPool), VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT);
    }
    CommandPool::~CommandPool() {
        vkDestroyCommandPool(device.GetDevice(), commandPool, nullptr);
    }
}
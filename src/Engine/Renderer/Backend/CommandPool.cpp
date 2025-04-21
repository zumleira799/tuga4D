#include "CommandPool.h"
#include <Engine/Renderer/Backend/Device.h>

namespace tuga4d::Engine::Renderer::Backend {
    CommandPool::CommandPool(Device& device, const std::string& debugName) : DeviceObject(device) {
        VkCommandPoolCreateInfo createInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        vkCreateCommandPool(device.GetDevice(), &createInfo, nullptr, &commandPool);
        CreateDebugInfo(debugName, reinterpret_cast<uint64_t>(commandPool), VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT);
    }
    CommandPool::~CommandPool() {
        vkDestroyCommandPool(device.GetDevice(), commandPool, nullptr);
    }
}
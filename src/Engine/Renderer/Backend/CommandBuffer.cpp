#include "CommandBuffer.h"
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Backend/CommandPool.h>
#include <stdexcept>

namespace tuga4d::Engine::Renderer::Backend {
    CommandBuffer::CommandBuffer(Device& device, const std::string& debugName) : DeviceObject(device) {
        owningCommandPool = device.RequestCommandPool();
        VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = owningCommandPool->GetCommandPool();
        allocInfo.commandBufferCount = 1;
        if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer!");
        }
        CreateDebugInfo(debugName, reinterpret_cast<uint64_t>(commandBuffer), VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT);
    }
    CommandBuffer::~CommandBuffer() {
        vkFreeCommandBuffers(device.GetDevice(), owningCommandPool->GetCommandPool(), 1, &commandBuffer);
        device.ReleaseCommandPool(owningCommandPool);
    }
}

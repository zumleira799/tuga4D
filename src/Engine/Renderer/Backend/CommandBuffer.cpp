#include "CommandBuffer.h"
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Backend/CommandPool.h>

namespace tuga4d::Engine::Renderer::Backend {
    CommandBuffer::CommandBuffer(Device& device, const std::string& debugName) : DeviceObject(device) {


        CreateDebugInfo(debugName, reinterpret_cast<uint64_t>(commandBuffer), VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT);
    }
    CommandBuffer::~CommandBuffer() {
        vkFreeCommandBuffers(device.GetDevice(), owningCommandPool->GetCommandPool(), 1, &commandBuffer);
    }
}

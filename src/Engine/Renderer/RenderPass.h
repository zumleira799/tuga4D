#pragma once
#include <Core/Util.h>
#include <volk.h>
#include <vector>
#include <string>

namespace tuga4d::Engine::Renderer {
    namespace Backend {
        class CommandBuffer;
    }
    class RenderTarget;
    class RenderPass {
    public:
        RenderPass(const std::string& name, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
        ~RenderPass();

        void ResetTargets();

        void AddColorTarget(RenderTarget& colorTarget);
        void SetDepthTarget(RenderTarget& depthTarget);
        void Begin(Backend::CommandBuffer* commandBuffer, VkExtent2D extent, VkOffset2D offset = VkOffset2D());
        void End(Backend::CommandBuffer* commandBuffer);
    private:
        std::vector<VkRenderingAttachmentInfo> colorAttachments;
        VkRenderingAttachmentInfo depthAttachment;
        bool bUseDepthTarget = false;

        std::string debugName;

        VkDebugMarkerMarkerInfoEXT debugMarkerInfo{ VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT };
    };
}
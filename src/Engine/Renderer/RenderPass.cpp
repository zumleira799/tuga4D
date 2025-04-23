#include "RenderPass.h"
#include <Engine/Renderer/RenderTarget.h>
#include <Engine/Renderer/Backend/CommandBuffer.h>

using namespace tuga4d::Engine::Renderer::Backend;
namespace tuga4d::Engine::Renderer {
    RenderPass::RenderPass(const std::string& name, float r, float g, float b, float a) : debugName(name) {
        debugMarkerInfo.color[0] = r;
        debugMarkerInfo.color[1] = b;
        debugMarkerInfo.color[2] = g;
        debugMarkerInfo.color[3] = a;
        debugMarkerInfo.pMarkerName = debugName.c_str();
    }
    RenderPass::~RenderPass() {

    }
    void RenderPass::ResetTargets() {
        colorAttachments.clear();
        bUseDepthTarget = false;
    }
    void RenderPass::AddColorTarget(RenderTarget& colorTarget) {
        colorAttachments.emplace_back(colorTarget.GetAttachmentInfo());
    }
    void RenderPass::SetDepthTarget(RenderTarget& depthTarget) {
        depthAttachment = depthTarget.GetAttachmentInfo();
        bUseDepthTarget = true;
    }
    void RenderPass::Begin(CommandBuffer* commandBuffer, VkExtent2D extent, VkOffset2D offset) {
        if (vkCmdDebugMarkerBeginEXT != nullptr) {
            vkCmdDebugMarkerBeginEXT(commandBuffer->GetCommandBuffer(), &debugMarkerInfo);
        }
        VkRenderingInfo renderInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderInfo.pColorAttachments = colorAttachments.data();
        renderInfo.colorAttachmentCount = colorAttachments.size();
        renderInfo.layerCount = 1;
        if (bUseDepthTarget) {
            renderInfo.pDepthAttachment = &depthAttachment;
        }
        renderInfo.renderArea.extent = extent;
        renderInfo.renderArea.offset = offset;
        renderInfo.viewMask = 0;
        vkCmdBeginRendering(commandBuffer->GetCommandBuffer(), &renderInfo);

        VkViewport viewport;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 0.0f;
        viewport.x = offset.x;
        viewport.y = offset.y;
        viewport.width = extent.width;
        viewport.height = extent.height;
        vkCmdSetViewport(commandBuffer->GetCommandBuffer(), 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer->GetCommandBuffer(), 0, 1, &renderInfo.renderArea);
    }
    void RenderPass::End(CommandBuffer* commandBuffer) {
        vkCmdEndRendering(commandBuffer->GetCommandBuffer());
        if (vkCmdDebugMarkerEndEXT != nullptr) {
            vkCmdDebugMarkerEndEXT(commandBuffer->GetCommandBuffer());
        }
        /*
        VkImageMemoryBarrier2 imageBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };

        VkDependencyInfo dependency{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependency.pImageMemoryBarriers = imageBarriers;

        vkCmdPipelineBarrier2(commandBuffer->GetCommandBuffer(), &dependency)
        */
    }
}
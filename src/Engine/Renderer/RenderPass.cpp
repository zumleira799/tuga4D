#include "RenderPass.h"
#include <Engine/Renderer/RenderTarget.h>
#include <Engine/Renderer/RenderBarrier.h>
#include <Engine/Renderer/Backend/CommandBuffer.h>

using namespace tuga4d::Engine::Renderer::Backend;
namespace tuga4d::Engine::Renderer {
    RenderPass::RenderPass(const std::string& name, float r, float g, float b, float a) : debugName(name) {
        debugMarkerInfo.color[0] = r;
        debugMarkerInfo.color[1] = b;
        debugMarkerInfo.color[2] = g;
        debugMarkerInfo.color[3] = a;
        debugMarkerInfo.pMarkerName = debugName.c_str();

        inBarrier = new RenderBarrier(name + " RT In-Barrier");
        outBarrier = new RenderBarrier(name + " RT Out-Barrier");
    }
    RenderPass::~RenderPass() {
        delete inBarrier;
        delete outBarrier;
    }
    void RenderPass::ResetTargets() {
        colorAttachments.clear();
        inBarrier->ResetBarriers();
        outBarrier->ResetBarriers();
        bUseDepthTarget = false;
    }
    void RenderPass::AddColorTarget(RenderTarget& colorTarget) {
        colorAttachments.emplace_back(colorTarget.GetAttachmentInfo());
        BarrierMask srcMask{};
        BarrierMask midMask{};
        BarrierMask dstMask{};
        if (colorTarget.GetAttachmentInfo().loadOp == VK_ATTACHMENT_LOAD_OP_LOAD) {
            srcMask.accessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
            srcMask.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        } else {
            srcMask.accessMask = VK_ACCESS_2_NONE;
            srcMask.stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        }
        midMask.accessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        midMask.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

        // TODO, set the optimal access and stage masks per image layout
        if (colorTarget.GetTargetImageLayout() == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            dstMask.accessMask = VK_ACCESS_2_SHADER_READ_BIT;
            dstMask.stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        } else if (colorTarget.GetTargetImageLayout() == VK_IMAGE_LAYOUT_GENERAL) {
            dstMask.accessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
            dstMask.stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        } else {
            dstMask.accessMask = VK_ACCESS_2_NONE;
            dstMask.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
        }
        inBarrier->AddImageBarrier(srcMask, midMask,
            colorTarget.GetReferencingImageView(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            colorTarget.GetAttachmentInfo().imageLayout
        );
        outBarrier->AddImageBarrier(midMask, dstMask,
            colorTarget.GetReferencingImageView(),
            colorTarget.GetAttachmentInfo().imageLayout,
            colorTarget.GetTargetImageLayout()
        );
    }
    void RenderPass::SetDepthTarget(RenderTarget& depthTarget) {
        depthAttachment = depthTarget.GetAttachmentInfo();
        bUseDepthTarget = true;

        BarrierMask srcMask{};
        BarrierMask midMask{};
        BarrierMask dstMask{};
        if (depthTarget.GetAttachmentInfo().loadOp == VK_ATTACHMENT_LOAD_OP_LOAD) {
            srcMask.accessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            srcMask.stageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        } else {
            srcMask.accessMask = VK_ACCESS_2_NONE;
            srcMask.stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        }
        midMask.accessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | 
            VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        midMask.stageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        if (depthTarget.GetTargetImageLayout() == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            dstMask.accessMask = VK_ACCESS_2_SHADER_READ_BIT;
            dstMask.stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        } else if (depthTarget.GetTargetImageLayout() == VK_IMAGE_LAYOUT_GENERAL) {
            dstMask.accessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
            dstMask.stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        } else {
            dstMask.accessMask = VK_ACCESS_2_NONE;
            dstMask.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
        }
        inBarrier->AddImageBarrier(srcMask, midMask,
            depthTarget.GetReferencingImageView(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            depthTarget.GetAttachmentInfo().imageLayout
        );
        outBarrier->AddImageBarrier(midMask, dstMask,
            depthTarget.GetReferencingImageView(),
            depthTarget.GetAttachmentInfo().imageLayout,
            depthTarget.GetTargetImageLayout()
        );
    }
    void RenderPass::Begin(CommandBuffer& commandBuffer, VkExtent2D extent, VkOffset2D offset) {
        inBarrier->FlushBarriers(commandBuffer);

        if (vkCmdDebugMarkerBeginEXT != nullptr && !debugName.empty()) {
            vkCmdDebugMarkerBeginEXT(commandBuffer.GetCommandBuffer(), &debugMarkerInfo);
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
        vkCmdBeginRendering(commandBuffer.GetCommandBuffer(), &renderInfo);

        VkViewport viewport;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 0.0f;
        viewport.x = offset.x;
        viewport.y = offset.y;
        viewport.width = extent.width;
        viewport.height = extent.height;
        vkCmdSetViewport(commandBuffer.GetCommandBuffer(), 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer.GetCommandBuffer(), 0, 1, &renderInfo.renderArea);
    }
    void RenderPass::End(CommandBuffer& commandBuffer) {
        vkCmdEndRendering(commandBuffer.GetCommandBuffer());
        if (vkCmdDebugMarkerEndEXT != nullptr && !debugName.empty()) {
            vkCmdDebugMarkerEndEXT(commandBuffer.GetCommandBuffer());
        }
        outBarrier->FlushBarriers(commandBuffer);
    }
}
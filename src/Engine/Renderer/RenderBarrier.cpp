#include "RenderBarrier.h"
#include <Engine/Renderer/Backend/CommandBuffer.h>
#include <Engine/Renderer/Backend/Image.h>
#include <Engine/Renderer/Backend/ImageView.h>
#include <Engine/Renderer/Backend/Buffer.h>

namespace tuga4d::Engine::Renderer {
    RenderBarrier::RenderBarrier(const std::string& debugName) : debugName(debugName) {

    }
    RenderBarrier::~RenderBarrier() {

    }

    void RenderBarrier::ResetBarriers() {
        bufferMemoryBarriers.clear();
        imageMemoryBarriers.clear();
        memoryBarriers.clear();
    }
    void RenderBarrier::AddImageBarrier(const BarrierMask& srcMask, const BarrierMask& dstMask,
        Backend::Image& image, const VkImageSubresourceRange& subresourceRange, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkImageMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
        barrier.srcAccessMask = srcMask.accessMask;
        barrier.srcStageMask = srcMask.stageMask;
        barrier.dstAccessMask = dstMask.accessMask;
        barrier.dstStageMask = dstMask.stageMask;
        barrier.dstQueueFamilyIndex = 0;
        barrier.srcQueueFamilyIndex = 0;

        barrier.subresourceRange = subresourceRange;
        barrier.image = image.GetImage();
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        imageMemoryBarriers.emplace_back(barrier);
    }
    void RenderBarrier::AddImageBarrier(const BarrierMask& srcMask, const BarrierMask& dstMask, Backend::ImageView& view,
        VkImageLayout oldLayout, VkImageLayout newLayout) {
        AddImageBarrier(srcMask, dstMask, view.GetReferencedImage(), view.GetSubresourceRange(), oldLayout, newLayout);
    }
    void RenderBarrier::AddBufferBarrier(const BarrierMask& srcMask, const BarrierMask& dstMask, Backend::Buffer& buffer,
        VkDeviceSize offset, VkDeviceSize size) {
        VkBufferMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2 };
        barrier.srcAccessMask = srcMask.accessMask;
        barrier.srcStageMask = srcMask.stageMask;
        barrier.dstAccessMask = dstMask.accessMask;
        barrier.dstStageMask = dstMask.stageMask;
        barrier.dstQueueFamilyIndex = 0;
        barrier.srcQueueFamilyIndex = 0;

        barrier.buffer = buffer.GetBuffer();
        barrier.offset = offset;
        barrier.size = size == VK_WHOLE_SIZE ? (buffer.GetBufferSize() - offset) : size;
        assert((barrier.offset + barrier.size) <= buffer.GetBufferSize());

        bufferMemoryBarriers.emplace_back(barrier);
    }
    void RenderBarrier::AddMemoryBarrier(const BarrierMask& srcMask, const BarrierMask& dstMask) {
        VkMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
        barrier.srcAccessMask = srcMask.accessMask;
        barrier.srcStageMask = srcMask.stageMask;
        barrier.dstAccessMask = dstMask.accessMask;
        barrier.dstStageMask = dstMask.stageMask;
        memoryBarriers.emplace_back(barrier);
    }

    void RenderBarrier::FlushBarriers(Backend::CommandBuffer& commandBuffer) {
        VkDependencyInfo dependencyInfo{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependencyInfo.pBufferMemoryBarriers = bufferMemoryBarriers.data();
        dependencyInfo.bufferMemoryBarrierCount = bufferMemoryBarriers.size();
        dependencyInfo.pImageMemoryBarriers = imageMemoryBarriers.data();
        dependencyInfo.imageMemoryBarrierCount = imageMemoryBarriers.size();
        dependencyInfo.pMemoryBarriers = memoryBarriers.data();
        dependencyInfo.memoryBarrierCount = memoryBarriers.size();

        if (vkCmdDebugMarkerInsertEXT && !debugName.empty()) {
            VkDebugMarkerMarkerInfoEXT markerInfo{VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT};
            markerInfo.pMarkerName = debugName.c_str();
            vkCmdDebugMarkerInsertEXT(commandBuffer.GetCommandBuffer(), &markerInfo);
        }
        vkCmdPipelineBarrier2(commandBuffer.GetCommandBuffer(), &dependencyInfo);
    }
}
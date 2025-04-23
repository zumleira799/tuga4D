#pragma once
#include <Core/Util.h>
#include <volk.h>
#include <vector>
#include <string>

namespace tuga4d::Engine::Renderer {
    namespace Backend {
        class CommandBuffer;
        class Image;
        class ImageView;
        class Buffer;
    }
    struct BarrierMask {
        VkPipelineStageFlags2 stageMask;
        VkAccessFlags2 accessMask;
    };
    class RenderBarrier : NoCopy, NoMove {
    public:
        RenderBarrier(const std::string& debugName);
        ~RenderBarrier();

        void ResetBarriers();

        void AddImageBarrier(const BarrierMask& srcMask, const BarrierMask& dstMask,
            Backend::Image& image, const VkImageSubresourceRange& subresourceRange, VkImageLayout oldLayout, VkImageLayout newLayout);
        void AddImageBarrier(const BarrierMask& srcMask, const BarrierMask& dstMask, Backend::ImageView& view,
            VkImageLayout oldLayout, VkImageLayout newLayout);
        void AddBufferBarrier(const BarrierMask& srcMask, const BarrierMask& dstMask, Backend::Buffer& buffer, 
            VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
        void AddMemoryBarrier(const BarrierMask& srcMask, const BarrierMask& dstMask);

        void FlushBarriers(Backend::CommandBuffer& commandBuffer);
    private:
        std::string debugName;

        std::vector<VkImageMemoryBarrier2> imageMemoryBarriers{};
        std::vector<VkBufferMemoryBarrier2> bufferMemoryBarriers{};
        std::vector<VkMemoryBarrier2> memoryBarriers{};
    };
}
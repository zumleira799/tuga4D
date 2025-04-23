#pragma once
#include <Core/Util.h>
#include <volk.h>

namespace tuga4d::Engine::Renderer {
    namespace Backend {
        class ImageView;
    }
    class RenderTarget : NoCopy, NoMove {
    public:
        enum class Type {
            Color,
            DepthStencil,
            DepthStencilReadonly
        };
        struct Builder {
            Builder(Backend::ImageView& imageView, Type type, VkImageLayout dstImageLayout);
            Builder& SetLoadStoreOp(VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp);
            Builder& SetClearColor(float r, float g, float b, float a);
            Builder& SetClearColor(int32_t r, int32_t g, int32_t b, int32_t a);
            Builder& SetClearColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a);
            Builder& SetClearDepth(float depthClear);
            Builder& SetClearStencil(uint8_t stencilClear);
            Builder& SetResolveTarget(Backend::ImageView& resolveImageView, VkImageLayout dstImageLayout,
                VkResolveModeFlagBits resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT);
            RenderTarget* Build();
        private:
            VkRenderingAttachmentInfo attachmentInfo{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
            VkImageLayout dstImageLayout;
            Backend::ImageView* view;
        };
        RenderTarget(VkRenderingAttachmentInfo attachmentInfo, Backend::ImageView& view, VkImageLayout finalImageLayout);
        ~RenderTarget();

        VkImageLayout GetTargetImageLayout() const {
            return finalImageLayout;
        }
        const VkRenderingAttachmentInfo& GetAttachmentInfo() {
            return attachmentInfo;
        }
        Backend::ImageView& GetReferencingImageView() {
            return viewRef;
        }
    private:
        VkRenderingAttachmentInfo attachmentInfo;
        VkImageLayout finalImageLayout;
        Backend::ImageView& viewRef;
    };
}
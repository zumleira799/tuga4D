#include "RenderTarget.h"
#include <Engine/Renderer/Backend/ImageView.h>

namespace tuga4d::Engine::Renderer {
    RenderTarget::Builder::Builder(Backend::ImageView& imageView, 
        RenderTarget::Type type, VkImageLayout dstImageLayout) : dstImageLayout(dstImageLayout), view(&imageView) {
        attachmentInfo.imageView = imageView.GetImageView();
        switch (type) {
        case Type::Color:
            attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            break;
        case Type::DepthStencil:
            attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            break;
        case Type::DepthStencilReadonly:
            attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            break;
        }
    }
    RenderTarget::Builder& RenderTarget::Builder::SetLoadStoreOp(VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp) {
        attachmentInfo.loadOp = loadOp;
        attachmentInfo.storeOp = storeOp;
        return *this;
    }
    RenderTarget::Builder& RenderTarget::Builder::SetClearColor(float r, float g, float b, float a) {
        attachmentInfo.clearValue.color.float32[0] = r;
        attachmentInfo.clearValue.color.float32[1] = g;
        attachmentInfo.clearValue.color.float32[2] = b;
        attachmentInfo.clearValue.color.float32[3] = a;
        return *this;
    }
    RenderTarget::Builder& RenderTarget::Builder::SetClearColor(int32_t r, int32_t g, int32_t b, int32_t a) {
        attachmentInfo.clearValue.color.int32[0] = r;
        attachmentInfo.clearValue.color.int32[1] = g;
        attachmentInfo.clearValue.color.int32[2] = b;
        attachmentInfo.clearValue.color.int32[3] = a;
        return *this;
    }
    RenderTarget::Builder& RenderTarget::Builder::SetClearColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
        attachmentInfo.clearValue.color.uint32[0] = r;
        attachmentInfo.clearValue.color.uint32[1] = g;
        attachmentInfo.clearValue.color.uint32[2] = b;
        attachmentInfo.clearValue.color.uint32[3] = a;
        return *this;
    }
    RenderTarget::Builder& RenderTarget::Builder::SetClearDepth(float depthClear) {
        attachmentInfo.clearValue.depthStencil.depth = depthClear;
        return *this;
    }
    RenderTarget::Builder& RenderTarget::Builder::SetClearStencil(uint8_t stencilClear) {
        attachmentInfo.clearValue.depthStencil.stencil = static_cast<uint32_t>(stencilClear);
        return *this;
    }
    RenderTarget::Builder& RenderTarget::Builder::SetResolveTarget(Backend::ImageView& resolveImageView, 
        VkImageLayout dstImageLayout, VkResolveModeFlagBits resolveMode) {
        attachmentInfo.resolveImageView = resolveImageView.GetImageView();
        attachmentInfo.resolveImageLayout = dstImageLayout;
        attachmentInfo.resolveMode = resolveMode;
        return *this;
    }
    RenderTarget* RenderTarget::Builder::Build() {
        return new RenderTarget(attachmentInfo, *view, dstImageLayout);
    }
    RenderTarget::RenderTarget(VkRenderingAttachmentInfo attachmentInfo, Backend::ImageView& view, VkImageLayout finalImageLayout)
     : attachmentInfo(attachmentInfo), viewRef(view), finalImageLayout(finalImageLayout) {

    }
    RenderTarget::~RenderTarget() {

    }
}
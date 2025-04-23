#include "ImageView.h"
#include <Core/Logger.h>
#include <cassert>

namespace tuga4d::Engine::Renderer::Backend {
    ImageView::Builder::Builder(Image& image, VkImageViewType imageType) : image(&image) {
        createInfo.image = image.GetImage();
        createInfo.format = image.GetFormat();
        createInfo.viewType = imageType;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.layerCount = image.GetLayerCount();
        createInfo.subresourceRange.levelCount = image.GetMipCount();
    }
    ImageView::Builder& ImageView::Builder::SetLayerRange(uint32_t firstLayer, uint32_t numLayers) {
        assert(numLayers > 0 && (firstLayer + numLayers) <= image->GetLayerCount());
        createInfo.subresourceRange.baseArrayLayer = firstLayer;
        createInfo.subresourceRange.layerCount = numLayers;
        return *this;
    }
    ImageView::Builder& ImageView::Builder::SetMipRange(uint32_t firstMip, uint32_t numMips) {
        assert(numMips > 0 && (firstMip + numMips) <= image->GetMipCount());
        createInfo.subresourceRange.baseMipLevel = firstMip;
        createInfo.subresourceRange.levelCount = numMips;
        return *this;
    }
    ImageView::Builder& ImageView::Builder::AddAspectFlag(VkImageAspectFlags aspects) {
        createInfo.subresourceRange.aspectMask |= aspects;
        return *this;
    }
    ImageView::Builder& ImageView::Builder::MapComponents(VkComponentSwizzle r, VkComponentSwizzle g,
        VkComponentSwizzle b, VkComponentSwizzle a) {
        createInfo.components.r = r;
        createInfo.components.g = g;
        createInfo.components.b = b;
        createInfo.components.a = a;
        return *this;
    }
    ImageView* ImageView::Builder::Build(Device& device, const std::string& debugName) {
        return new ImageView(device, debugName, *image, createInfo);
    }
    ImageView::ImageView(Device& device, const std::string& debugName, Image& image, const VkImageViewCreateInfo& createInfo)
        : DeviceObject(device), createInfo(createInfo), imageRef(image) {
        VkResult result = vkCreateImageView(device.GetDevice(), &createInfo, nullptr, &imageView);
        if (result != VK_SUCCESS) {
            Logger::Error("Failed to create image view %s %p", GetDebugName(), this);
        }
        CreateDebugInfo(debugName, (uint64_t)imageView, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT);
    }
    ImageView::~ImageView() {
        vkDestroyImageView(device.GetDevice(), imageView, nullptr);
    }
}
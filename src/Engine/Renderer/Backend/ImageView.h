#pragma once
#include <Engine/Renderer/Backend/Image.h>

namespace tuga4d::Engine::Renderer::Backend {
    class ImageView : public DeviceObject {
    public:
        struct Builder {
            Builder(Image& image, VkImageViewType imageViewType);
            Builder& SetLayerRange(uint32_t firstLayer, uint32_t numLayers);
            Builder& SetMipRange(uint32_t firstMip, uint32_t numMips);
            Builder& AddAspectFlag(VkImageAspectFlags aspects);
            Builder& MapComponents(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a);
            ImageView* Build(Device& device, const char* debugName);
        private:
            VkImageViewCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            Image* image;
        };
        ImageView(Device& device, const char* debugName, const VkImageViewCreateInfo& createInfo);
        ~ImageView();

        bool IsOk() const {
            return imageView != VK_NULL_HANDLE;
        }

        VkImageView GetImageView() {
            return imageView;
        }
        VkImageViewType GetViewType() const {
            return createInfo.viewType;
        }
        VkFormat GetFormat() const {
            return createInfo.format;
        }
        uint32_t GetLayerCount() const {
            return createInfo.subresourceRange.layerCount;
        }
        uint32_t GetMipCount() const {
            return createInfo.subresourceRange.levelCount;
        }
        uint32_t GetBaseLayerIndex() const {
            return createInfo.subresourceRange.baseArrayLayer;
        }
        uint32_t GetBaseMipIndex() const {
            return createInfo.subresourceRange.baseMipLevel;
        }
    private:
        Device& device;
        VkImageViewCreateInfo createInfo;
        VkImageView imageView;
    };
}
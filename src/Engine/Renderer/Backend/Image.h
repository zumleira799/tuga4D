#pragma once
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Backend/DeviceObject.h>
#include <vma/vk_mem_alloc.h>

namespace tuga4d::Engine::Renderer::Backend {
    class Image : public DeviceObject {
    public:
        struct Builder {
            Builder(VkImageType imageType);
            Builder& SetExtent(uint32_t width = 1, uint32_t height = 1, uint32_t depth = 1);
            Builder& SetFormat(VkFormat format);
            Builder& SetLayerCount(uint32_t numLayers);
            Builder& SetMipCount(uint32_t numMips);
            Builder& SetSampleCount(VkSampleCountFlagBits samples);
            Builder& AddUsageFlags(VkImageUsageFlags usages);
            Image* Build(Device& device, const char* debugName);
        private:
            VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        };
        Image(Device& device, const char* debugName, const VkImageCreateInfo& createInfo);
        ~Image();

        bool IsOk() const {
            return image != VK_NULL_HANDLE;
        }

        VkImage GetImage() {
            return image;
        }
        VkFormat GetFormat() const {
            return createInfo.format;
        }
        VkExtent3D GetExtent() const {
            return createInfo.extent;
        }
        VkImageType GetImageType() const {
            return createInfo.imageType;
        }
        uint32_t GetWidth() const {
            return createInfo.extent.width;
        }
        uint32_t GetHeight() const {
            return createInfo.extent.height;
        }
        uint32_t GetDepth() const {
            return createInfo.extent.depth;
        }
        uint32_t GetLayerCount() const {
            return createInfo.arrayLayers;
        }
        uint32_t GetMipCount() const {
            return createInfo.mipLevels;
        }
    private:
        Device& device;
        VkImageCreateInfo createInfo;
        VkImage image;
        VmaAllocation memory;
    };
}
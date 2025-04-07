#include "Image.h"
#include <Core/Logger.h>
#include <cassert>

namespace tuga4d::Engine::Renderer::Backend {
    Image::Builder::Builder(VkImageType imageType) {
        createInfo.arrayLayers = 1;
        createInfo.mipLevels = 1;
        createInfo.extent = { 1,1,1 };
        createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.imageType = imageType;
    }
    Image::Builder& Image::Builder::SetExtent(uint32_t width, uint32_t height, uint32_t depth) {
        assert(width > 0 && height > 0 && depth > 0);
        createInfo.extent = {width, height, depth};
        return *this;
    }
    Image::Builder& Image::Builder::SetFormat(VkFormat format) {
        createInfo.format = format;
        return *this;
    }
    Image::Builder& Image::Builder::SetLayerCount(uint32_t numLayers) {
        createInfo.arrayLayers = numLayers;
        return *this;
    }
    Image::Builder& Image::Builder::SetMipCount(uint32_t numMips) {
        createInfo.mipLevels = numMips;
        return *this;
    }
    Image::Builder& Image::Builder::SetSampleCount(VkSampleCountFlagBits samples) {
        createInfo.samples = samples;
        return *this;
    }
    Image::Builder& Image::Builder::AddUsageFlags(VkImageUsageFlags usages) {
        createInfo.usage |= usages;
        return *this;
    }
    Image* Image::Builder::Build(Device& device, const std::string& debugName) {
        return new Image(device, debugName, createInfo);
    }
    Image::Image(Device& device, const std::string& debugName, const VkImageCreateInfo& createInfo)
        : device(device), createInfo(createInfo) {

        // TODO, come back to this to implement the rest of the struct
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        assert(false && "FIXME");

        VkResult result = vmaCreateImage(device.GetMemoryAllocator(), &createInfo, &allocInfo, &image, &memory, nullptr);
        if (result != VK_SUCCESS) {
            Logger::Error("Failed to create image %s %p", GetDebugName(), this);
        }
        CreateDebugInfo(device, debugName, (uint64_t)image, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT);
    }
    void Image::OnDestruct() {
        vmaFreeMemory(device.GetMemoryAllocator(), memory);
        vkDestroyImage(device.GetDevice(), image, nullptr);
    }
}
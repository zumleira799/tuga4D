#include "Buffer.h"
#include <Core/Logger.h>

namespace tuga4d::Engine::Renderer::Backend {
    VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    Buffer::Builder::Builder() {
    }

    Buffer::Builder& Buffer::Builder::SetMemoryUsage(BufferMemoryUsage usage) {
        switch (usage) {
        case BufferMemoryUsage::Auto:
            memoryUsage = VMA_MEMORY_USAGE_AUTO;
            break;
        case BufferMemoryUsage::Host:
            memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            break;
        case BufferMemoryUsage::Device:
            memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            break;
        default:
            assert(false);
        }
        return *this;
    }
    Buffer::Builder& Buffer::Builder::AddUsageFlags(VkBufferUsageFlags usages) {
        createInfo.usage |= usages;
        return *this;
    }
    Buffer* Buffer::Builder::Build(Device& device, const char* debugName) {
        return new Buffer(device, debugName, createInfo);
    }
    Buffer::Buffer(Device& device, const char* debugName, const VkBufferCreateInfo& createInfo)
        : device(device) {
        // hey vasco, it's me tristan, i actually forgot what this is for, 
        // but im pretty sure you dont ever need it to be anything other than 1
        VkDeviceSize minOffsetAlignment = 1;
        alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;

        // TODO, unlike images, buffers make sense to allocate host side. Make sure to implement properly.
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;

        VkResult result = vmaCreateBuffer(device.GetMemoryAllocator(), &createInfo, &allocInfo, &buffer, &memory, nullptr);
        if (result != VK_SUCCESS) {
            Logger::Error("Failed to create buffer %s %p", GetDebugName(), this);
        }
        CreateDebugInfo(device, debugName, (uint64_t)buffer, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT);
    }
    Buffer::~Buffer() {
        Unmap();
        vmaFreeMemory(device.GetMemoryAllocator(), memory);
        vkDestroyBuffer(device.GetDevice(), buffer, nullptr);
    }
    
    VkResult Buffer::Map(VkDeviceSize offset, VkDeviceSize size) {
        assert(!mappedMemory && "Memory already mapped!");
        assert(buffer && memory && "Called map on buffer before create");
        return vmaMapMemory(device.GetMemoryAllocator(), memory, &mappedMemory);
    }

    void Buffer::Unmap() {
        if (mappedMemory) {
            vmaUnmapMemory(device.GetMemoryAllocator(), memory);
            mappedMemory = nullptr;
        }
    }

    void Buffer::WriteToBuffer(const void* data, VkDeviceSize offset, VkDeviceSize size) {
        assert(mappedMemory && "Cannot copy to unmapped buffer");

        char* memOffset = (char*)mappedMemory;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }

   
    VkResult Buffer::Flush(VkDeviceSize offset, VkDeviceSize size) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory->GetMemory();
        mappedRange.offset = memory->GetOffset() + offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(engineDevice.device(), 1, &mappedRange);
    }

    VkResult Buffer::Invalidate(VkDeviceSize offset, VkDeviceSize size) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory->GetMemory();
        mappedRange.offset = memory->GetOffset() + offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(device.GetDevice(), 1, &mappedRange);
    }

    void Buffer::WriteToIndex(const void* data, int index, VkDeviceSize offset, VkDeviceSize size) {
        WriteToBuffer(data, index * alignmentSize + offset, size);
    }

    VkResult Buffer::FlushIndex(int index) { 
        return Flush(index * alignmentSize, alignmentSize); 
    }

    VkResult Buffer::InvalidateIndex(int index) {
        return Invalidate(index * alignmentSize, alignmentSize);
    }
}
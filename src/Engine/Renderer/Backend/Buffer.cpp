#include "Buffer.h"
#include <Core/Logger.h>
#include <cassert>
#include <cstring>

namespace tuga4d::Engine::Renderer::Backend {
    VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    Buffer::Builder::Builder(VkDeviceSize instanceSize, uint32_t instanceCount) : instanceCount(instanceCount){
        createInfo.size = instanceSize * instanceCount;
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
    Buffer::Builder& Buffer::Builder::PersistenlyMap() {
        allocationFlags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
        return *this;
    }
    Buffer::Builder& Buffer::Builder::ForceDedicatedMemory() {
        allocationFlags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        return *this;
    }
    Buffer* Buffer::Builder::Build(Device& device, const std::string& debugName) {
        return new Buffer(device, debugName, createInfo, memoryUsage, allocationFlags, instanceCount);
    }
    Buffer::Buffer(Device& device, const std::string& debugName, const VkBufferCreateInfo& createInfo, VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags allocFlags, uint32_t instanceCount)
        : DeviceObject(device) {

        instanceSize = createInfo.size / static_cast<VkDeviceSize>(instanceCount);
        assert(instanceSize * instanceCount == createInfo.size);

        VkDeviceSize minOffsetAlignment = 1; // if this casues validation errors when using funky offsets then lmn
        alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;

        this->memoryUsage = memoryUsage;
        // TODO, unlike images, buffers make sense to allocate host side. Make sure to implement properly.
        VmaAllocationCreateInfo allocCreate{};
        allocCreate.usage = memoryUsage;
        allocCreate.memoryTypeBits = UINT32_MAX;
        allocCreate.flags = allocFlags;
        VmaAllocationInfo allocInfo;
        VkResult result = vmaCreateBuffer(device.GetMemoryAllocator(), &createInfo, &allocCreate, &buffer, &memory, &allocInfo);
        if (result != VK_SUCCESS) {
            Logger::Error("Failed to create buffer %s %p", GetDebugName(), this);
        }
        mappedMemory = allocInfo.pMappedData;
        CreateDebugInfo(debugName, (uint64_t)buffer, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT);
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
        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(device.GetMemoryAllocator(), memory, &allocInfo);

        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = allocInfo.deviceMemory;
        mappedRange.offset = allocInfo.offset + offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(device.GetDevice(), 1, &mappedRange);
    }

    VkResult Buffer::Invalidate(VkDeviceSize offset, VkDeviceSize size) {
        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(device.GetMemoryAllocator(), memory, &allocInfo);

        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = allocInfo.deviceMemory;
        mappedRange.offset = allocInfo.offset + offset;
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
    Buffer::CopyBufferToBufferBuilder::CopyBufferToBufferBuilder(Buffer& other, VkDeviceSize size) {

    }
    Buffer::CopyBufferToBufferBuilder& Buffer::CopyBufferToBufferBuilder::SetSrcOffset(VkDeviceSize offset) {
        // TODO: insert return statement here
    }
    Buffer::CopyBufferToBufferBuilder& Buffer::CopyBufferToBufferBuilder::SetDstOffset(VkDeviceSize offset) {
        // TODO: insert return statement here
    }
    Buffer::CopyBufferToBufferBuilder& Buffer::CopyBufferToBufferBuilder::NextRegion(Buffer& other, VkDeviceSize size) {
        // TODO: insert return statement here
    }
    void Buffer::CopyBufferToBufferBuilder::Copy(CommandBuffer& commandBuffer) {
    }
}
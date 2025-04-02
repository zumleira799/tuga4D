#pragma once
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Backend/DeviceObject.h>
#include <vma/vk_mem_alloc.h>
#include <cstdlib>

namespace tuga4d::Engine::Renderer::Backend {
    enum class BufferMemoryUsage {
        Auto = 0,
        Host, CPU = Host,
        Device, GPU = Device
    };
    class Buffer : public DeviceObject {
    public:
        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        struct Builder {
            Builder();
            
            Builder& SetMemoryUsage(BufferMemoryUsage usage);
            Builder& AddUsageFlags(VkBufferUsageFlags usages);
            Buffer* Build(Device& device, const char* debugName);
        private:
            VkBufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
            VmaMemoryUsage memoryUsage;
        };
        Buffer(Device& device, const char* debugName, const VkBufferCreateInfo& createInfo);
        ~Buffer();
        
        /**
         * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
         *
         * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
         * buffer range.
         * @param offset (Optional) Byte offset from beginning
         *
         * @return VkResult of the buffer mapping call
         */
        VkResult Map(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        /**
         * Unmap a mapped memory range
         *
         * @note Does not return a result as vkUnmapMemory can't fail
         */
        void Unmap();

        /**
         * Copies the specified data to the mapped buffer. Default value writes whole buffer range
         *
         * @param data Pointer to the data to copy
         * @param size Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
         * range.
         * @param offset Byte offset from beginning of mapped region
         *
         */
        void WriteToBuffer(const void* data, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
        
        /**
        * Flush a memory range of the buffer to make it visible to the device
        *
        * @note Only required for non-coherent memory
        *
        * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
        * complete buffer range.
        * @param offset (Optional) Byte offset from beginning
        *
        * @return VkResult of the flush call
        */
        VkResult Flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        /**
         * Invalidate a memory range of the buffer to make it visible to the host
         *
         * @note Only required for non-coherent memory
         *
         * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
         * the complete buffer range.
         * @param offset (Optional) Byte offset from beginning
         *
         * @return VkResult of the invalidate call
         */
        VkResult Invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        /**
         * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
         *
         * @param data Pointer to the data to copy
         * @param index Used in offset calculation
         *
         */
        void WriteToIndex(const void* data, int index, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        /**
        *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
        *
        * @param index Used in offset calculation
        *
        */
        VkResult FlushIndex(int index);

        /**
         * Invalidate a memory range of the buffer to make it visible to the host
         *
         * @note Only required for non-coherent memory
         *
         * @param index Specifies the region to invalidate: index * alignmentSize
         *
         * @return VkResult of the invalidate call
         */
        VkResult InvalidateIndex(int index);

        void* GetMappedMemory() { 
            return (void*)((char*)mappedMemory + mappedOffset); 
        }
        void* GetMappedIndex(int index) { 
            return (void*)((char*)mappedMemory + mappedOffset + index * alignmentSize);
        }

        VkDeviceSize getInstanceSize() const {
            return instanceSize; 
        }
        VkDeviceSize getAlignmentSize() const { 
            return alignmentSize; 
        }
        VkBufferUsageFlags getUsageFlags() const { 
            return usageFlags; 
        }
        VkMemoryPropertyFlags getMemoryPropertyFlags() const { 
            return memoryPropertyFlags; 
        }
        VkDeviceSize getBufferSize() const { 
            return bufferSize; 
        }
        VkDeviceSize getIndexOffset(int index) const { 
            return index * alignmentSize; 
        }

        bool IsOk() const {
            return buffer != VK_NULL_HANDLE;
        }
        VkBuffer GetBuffer() {
            return buffer;
        }
        size_t GetInstanceCount() const { 
            return instanceCount;
        }
    private:
        void* mappedMemory = nullptr;
        VkDeviceSize bufferSize;
        size_t instanceCount;
        VkDeviceSize instanceSize;
        VkDeviceSize alignmentSize;
        
        Device& device;
        VkBuffer buffer;
        VmaAllocation memory;
    };
}
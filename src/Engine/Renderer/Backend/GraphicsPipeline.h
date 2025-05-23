#pragma once
#include <Engine/Renderer/Backend/DeviceObject.h>
#include <vector>

namespace tuga4d::Engine::Renderer::Backend {
    class CommandBuffer;
    class GraphicsPipeline : public DeviceObject {
    public:
        struct BlendInfo {
            BlendInfo() = default;
            VkBlendOp blendOp{};
            VkBlendFactor srcFactor{}, dstFactor{};

            static BlendInfo OpaqueBlend() {
                return BlendInfo{
                    VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO
                };
            }
            static BlendInfo AlphaBlendColor() {
                return BlendInfo{
                    VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
                };
            }
        };
        struct Builder {
            Builder(uint32_t colorAttachmentCount);

            Builder& AddShaderStage(const std::string& shaderFile, VkShaderStageFlagBits stage, Device& device);
            VkShaderModule createShaderModule(const std::vector<char>& code, Device& device);

            Builder& SetDepthStencil(bool depthTest, bool depthWrite, bool stencilTest);
            Builder& SetDepthCompare(VkCompareOp depthCompare);
            Builder& SetMSAA(VkSampleCountFlagBits samples, bool alphaToCoverage = false);
            Builder& AddVertexAttribute(const VkVertexInputAttributeDescription& description);
            Builder& AddVertexBinding(const VkVertexInputBindingDescription& description);

            Builder& SetColorWriteMask(size_t colorAttachment, VkColorComponentFlags components);
            Builder& SetBlendEnabled(size_t colorAttachment, bool enabled);
            Builder& SetColorBlendOp(size_t colorAttachment, const BlendInfo& blendInfo);
            Builder& SetAlphaBlendOp(size_t colorAttachment, const BlendInfo& blendInfo);

            GraphicsPipeline* Build(Device& device, const std::string& debugName);
        private:
            VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
            std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
            std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions{};
            std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions{};
            VkPipelineVertexInputStateCreateInfo vertexInputState{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
            VkPipelineTessellationStateCreateInfo tessellationState{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
            VkPipelineRasterizationStateCreateInfo rasterizationState{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
            VkPipelineMultisampleStateCreateInfo multisampleState{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
            VkPipelineDepthStencilStateCreateInfo depthStencilState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
            std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentStates{};
            VkPipelineColorBlendStateCreateInfo colorBlendState{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
            std::vector<VkDynamicState> dynamicStates{};
            VkPipelineDynamicStateCreateInfo dynamicState{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        };

        GraphicsPipeline(Device& device, const VkGraphicsPipelineCreateInfo& createInfo, const std::string& debugName);

        void Bind(CommandBuffer& commandBuffer);

        bool IsOk() const {
            return pipeline != VK_NULL_HANDLE;
        }
    protected:
        virtual ~GraphicsPipeline();
    private:
        VkPipeline pipeline = VK_NULL_HANDLE;
    };
}
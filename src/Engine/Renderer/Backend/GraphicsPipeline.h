#pragma once
#include <Engine/Renderer/Backend/DeviceObject.h>
#include <vector>

namespace tuga4d::Engine::Renderer::Backend {
    class CommandBuffer;
    class GraphicsPipeline : public DeviceObject {
    public:
        struct Builder {
            Builder();

            Builder& AddShaderStage(const std::string& shaderFile, VkShaderStageFlagBits stage);

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
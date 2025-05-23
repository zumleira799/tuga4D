#include "GraphicsPipeline.h"
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Backend/CommandBuffer.h>
#include <iostream>
#include <stdexcept>

namespace tuga4d::Engine::Renderer::Backend {
    GraphicsPipeline::Builder::Builder(uint32_t colorAttachmentCount) {
        colorAttachmentStates.resize(colorAttachmentCount);
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyState.primitiveRestartEnable = VK_FALSE;
		inputAssemblyState.flags = 0;

		//RASTERIZATION
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationState.lineWidth = 1.0f;
		rasterizationState.cullMode = VK_CULL_MODE_NONE;
		rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationState.depthBiasEnable = VK_FALSE;
		rasterizationState.depthBiasConstantFactor = 0.0f;  // Optional
		rasterizationState.depthBiasClamp = 0.0f;           // Optional
		rasterizationState.depthBiasSlopeFactor = 0.0f;     // Optional

		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.minSampleShading = 1.0f;           // Optional
		multisampleState.pSampleMask = nullptr;             // Optional
		multisampleState.alphaToCoverageEnable = VK_FALSE;  // Optional
		multisampleState.alphaToOneEnable = VK_FALSE;       // Optional

		for (auto& blendAttachment : colorAttachmentStates) {
			blendAttachment.colorWriteMask =
				VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT;
			blendAttachment.blendEnable = VK_FALSE;
			blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
			blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
			blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
			blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
			blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
			blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
		}

		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.logicOpEnable = VK_FALSE;
		colorBlendState.logicOp = VK_LOGIC_OP_COPY;  // Optional
		colorBlendState.attachmentCount = colorAttachmentCount;
		colorBlendState.pAttachments = colorBlendAttachments.data();
		colorBlendState.blendConstants[0] = 0.0f;  // Optional
		colorBlendState.blendConstants[1] = 0.0f;  // Optional
		colorBlendState.blendConstants[2] = 0.0f;  // Optional
		colorBlendState.blendConstants[3] = 0.0f;  // Optional

		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.minDepthBounds = 0.0f;  // Optional
		depthStencilState.maxDepthBounds = 1.0f;  // Optional
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front = {};  // Optional
		depthStencilState.back = {};   // Optional

		dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    }

    VkShaderModule GraphicsPipeline::Builder::createShaderModule(const std::vector<char>& code, Device& device){
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        vkCreateShaderModule(device.GetDevice(), &createInfo, NULL, &shaderModule);
        return shaderModule;
    }

    GraphicsPipeline::Builder& GraphicsPipeline::Builder::AddShaderStage(
        const std::string& shaderFile, VkShaderStageFlagBits stage, Device& device) {
        VkPipelineShaderStageCreateInfo stageInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        
        std::ifstream reader{ shaderFile, std::ios::binary );
        std::vector<char> shaderData(std::istreambuf_iterator<char>(reader), {});
        
        stageInfo.module = createShaderModule(shaderData, device);
        stageInfo.stage = stage;
        stageInfo.pName = "main";
        stageInfo.pNext = nullptr;
        stageInfo.pSpecializationInfo = nullptr;
        shaderStages.emplace_back(std::move(stageInfo));
        return *this;
    }

    GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetDepthStencil(bool depthTest, bool depthWrite, bool stencilTest) {
        depthStencilState.depthTestEnable = depthTest;
        depthStencilState.depthWriteEnable = depthWrite;
        depthStencilState.stencilTestEnable = stencilTest;
        return *this;
    }

    GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetDepthCompare(VkCompareOp depthCompare) {
        depthStencilState.depthCompareOp = depthCompare;
        return *this;
    }
    GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetMSAA(VkSampleCountFlagBits samples, bool alphaToCoverage = false){
        multisampleState.rasterizationSamples = samples;
        multisampleState.alphaToCoverage = alphaToCoverage;
        multisampleState.sampleShadingEnable = samples > VK_SAMPLE_COUNT_1_BIT;
        return *this;
    }
    GraphicsPipeline::Builder& GraphicsPipeline::Builder::AddVertexAttribute(const VkVertexInputAttributeDescription& description){
        
        return *this;
    }
    GraphicsPipeline::Builder& GraphicsPipeline::Builder::AddVertexBinding(const VkVertexInputBindingDescription& description){
        
        return *this;
    }

    GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetColorWriteMask(size_t colorAttachment, VkColorComponentFlags components) {
        assert(colorAttachment < colorAttachmentStates.size() && "attachment out of index range!");
        colorAttachmentStates[colorAttachment].colorWriteMask = components; 
        return *this;
    }
    GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetBlendEnabled(size_t colorAttachment, bool enabled){
        assert(colorAttachment < colorAttachmentStates.size() && "attachment out of index range!");
        colorAttachmentStates[colorAttachment].blendEnable = enabled; 
        
        return *this;
    }
    GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetColorBlendOp(size_t colorAttachment, const BlendInfo& blendInfo){
        assert(colorAttachment < colorAttachmentStates.size() && "attachment out of index range!");

        colorAttachmentStates[colorAttachment].srcColorBlendFactor = blendInfo.srcFactor; 
        colorAttachmentStates[colorAttachment].dstColorBlendFactor = blendInfo.dstFactor; 
        colorAttachmentStates[colorAttachment].colorBlendOp = blendInfo.blendOp;
        return *this;
    }
    GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetAlphaBlendOp(size_t colorAttachment, const BlendInfo& blendInfo){
        assert(colorAttachment < colorAttachmentStates.size() && "attachment out of index range!");

        colorAttachmentStates[colorAttachment].srcAlphaBlendFactor = blendInfo.srcFactor; 
        colorAttachmentStates[colorAttachment].dstAlphaBlendFactor = blendInfo.dstFactor; 
        colorAttachmentStates[colorAttachment].alphaBlendOp = blendInfo.blendOp;
        return *this;
    }

    GraphicsPipeline* GraphicsPipeline::Builder::Build(Device& device, const std::string& debugName) {
        vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
        vertexInputState.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
        vertexInputState.pVertexBindingDescriptions = vertexBindingDescriptions.data();
        vertexInputState.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
        dynamicState.pDynamicStates = dynamicStates.data();
        dynamicState.dynamicStateCount = dynamicStates.size();
        colorBlendState.pAttachments = colorAttachmentStates.data();
        colorBlendState.attachmentCount = colorAttachmentStates.size();

        createInfo.pColorBlendState = &colorBlendState;
        createInfo.pDepthStencilState = &depthStencilState;
        createInfo.pDynamicState = &dynamicState;
        createInfo.pInputAssemblyState = &inputAssemblyState;
        createInfo.pMultisampleState = &multisampleState;
        createInfo.pRasterizationState = &rasterizationState;
        createInfo.pTessellationState = &tessellationState;
        createInfo.pVertexInputState = &vertexInputState;
        createInfo.pViewportState = nullptr;
        createInfo.pStages = shaderStages.data();
        createInfo.stageCount = shaderStages.size();

        createInfo.subpass = 0;
        createInfo.renderPass = VK_NULL_HANDLE;
        
        GraphicsPipeline* pipeline = new GraphicsPipeline(device, createInfo, debugName);

        for (VkShaderStage& stage : shaderStages) {
            vkDestroyShaderModule(device.GetDevice(), stage.module, nullptr);
        }

        return pipeline;
    }

    GraphicsPipeline::GraphicsPipeline(Device& device, const VkGraphicsPipelineCreateInfo& createInfo,
        const std::string& debugName) : DeviceObject(device) {
        
        if (vkCreateGraphicsPipelines(device.GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline)) != VK_SUCCESS) {
            throw std::runtime_error("Failed to crate graphics pipeline!");
        }
        CreateDebugInfo(debugName, reinterpret_cast<uint64_t>(pipeline), VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT);
    }
    GraphicsPipeline::~GraphicsPipeline() {
        vkDestroyPipeline(device.GetDevice(), pipeline, nullptr);
    }
    
    void GraphicsPipeline::Bind(CommandBuffer& commandBuffer) {
        vkCmdBindPipeline(commandBuffer.GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }
}

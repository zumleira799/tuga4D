#include "GraphicsPipeline.h"
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Backend/CommandBuffer.h>
#include <stdexcept>

namespace tuga4d::Engine::Renderer::Backend {
    GraphicsPipeline::Builder::Builder() {
    }

    GraphicsPipeline::Builder& GraphicsPipeline::Builder::AddShaderStage(
        const std::string& shaderFile, VkShaderStageFlagBits stage) {
        VkPipelineShaderStageCreateInfo stageInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        

        shaderStages.emplace_back(stageInfo);
        return *this;
    }

    GraphicsPipeline* GraphicsPipeline::Builder::Build(Device& device, const std::string& debugName) {
        createInfo.pColorBlendState = &colorBlendState;

        return new GraphicsPipeline(device, createInfo, debugName);
    }

    GraphicsPipeline::GraphicsPipeline(Device& device, const VkGraphicsPipelineCreateInfo& createInfo,
        const std::string& debugName) : DeviceObject(device) {
        
        
        if (vkCreateGraphicsPipelines() != VK_SUCCESS) {
            throw std::runtime_error("Failed to crate graphics pipeline!");
        }
        CreateDebugInfo(debugName, reinterpret_cast<uint64_t>(pipeline), VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT);
    }
    GraphicsPipeline::~GraphicsPipeline() {
        vkDestroyPipeline();
    }
}

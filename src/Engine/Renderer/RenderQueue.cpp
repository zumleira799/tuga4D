#include "RenderQueue.h"
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Backend/CommandBuffer.h>

#include <stdexcept>
#include <cassert>

namespace tuga4d::Engine::Renderer {
    using namespace Backend;
    RenderQueue::RenderQueue(Device& device) : device(device) {
        CreateSynchronization();
    }
    RenderQueue::~RenderQueue() {
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroyFence(device.GetDevice(), inFlightFences[i], nullptr);
            vkDestroySemaphore(device.GetDevice(), renderFinishedSemaphore[i], nullptr);
        }
    }
    void RenderQueue::WaitFences() {
        vkWaitForFences(
            device.GetDevice(),
            1,
            &inFlightFences[frameIndex],
            VK_TRUE,
            UINT64_MAX
        );
    }
    void RenderQueue::SubmitCommand(Backend::CommandBuffer& command) {
        assert(command.IsOk());
        submittedCommands.push_back(command.GetCommandBuffer());
    }
    void RenderQueue::PresentSwapchain(Backend::Swapchain& swapchain) {
        assert(swapchain.IsOk());
        submittedSwapchains.push_back(swapchain.GetSwapchain());
        awaitImageAvailableSemaphores.push_back(swapchain.GetCurrentWaitSemaphore());
        imageIndices.push_back(swapchain.GetImageIndex());
    }

    void RenderQueue::Flush() {
        if (!inFlightFencesFirstSignal[frameIndex]) {
            vkWaitForFences(device.GetDevice(), 1, &inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
        } else {
            inFlightFencesFirstSignal[frameIndex] = false;
        }
        vkResetFences(device.GetDevice(), 1, &inFlightFences[frameIndex]);

        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphore[frameIndex];

        submitInfo.pCommandBuffers = submittedCommands.data();
        submitInfo.commandBufferCount = submittedCommands.size();

        if (!submittedSwapchains.empty()) {
            static VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = awaitImageAvailableSemaphores.size();
            submitInfo.pWaitSemaphores = awaitImageAvailableSemaphores.data();
            submitInfo.pWaitDstStageMask = waitStages;
        }

        vkQueueSubmit(device.GetQueue(), 1, &submitInfo, inFlightFences[frameIndex]);

        std::vector<VkResult> result(submittedSwapchains.size());
        if (!submittedSwapchains.empty()) {
            VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
            presentInfo.pImageIndices = imageIndices.data();
            presentInfo.pSwapchains = submittedSwapchains.data();
            presentInfo.swapchainCount = submittedSwapchains.size();
            presentInfo.pWaitSemaphores = &renderFinishedSemaphore[frameIndex];
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pResults = result.data();
            vkQueuePresentKHR(device.GetQueue(), &presentInfo);
        }

        submittedCommands.clear();
        submittedSwapchains.clear();
        awaitImageAvailableSemaphores.clear();
        imageIndices.clear();

        frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void RenderQueue::CreateSynchronization() {
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            inFlightFencesFirstSignal[i] = true;
            VkFenceCreateInfo fenceCreateInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            if (vkCreateFence(device.GetDevice(), &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create fence!");
            }
            VkSemaphoreCreateInfo semaphoreCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            if (vkCreateSemaphore(device.GetDevice(), &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create semaphore!");
            }
        }
    }
}

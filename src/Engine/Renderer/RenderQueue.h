#pragma once
#include <Engine/Renderer/Backend/Swapchain.h>

namespace tuga4d::Engine::Renderer {
    namespace Backend {
        class CommandBuffer;
    }
    class RenderQueue : NoCopy, NoMove {
    public:
        RenderQueue(Backend::Device& device);
        ~RenderQueue();

        void WaitFences();
        void SubmitCommand(Backend::CommandBuffer* command);
        void PresentSwapchain(Backend::Swapchain* swapchain);

        void Flush();

        uint32_t GetFrameIndex() {
            return frameIndex;
        }
    private:
        void CreateSynchronization();
    private:
        std::vector<VkCommandBuffer> submittedCommands{};
        std::vector<VkSwapchainKHR> submittedSwapchains{};
        std::vector<VkSemaphore> awaitImageAvailableSemaphores{};
        std::vector<uint32_t> imageIndices{};

        std::array<bool, Backend::MAX_FRAMES_IN_FLIGHT> inFlightFencesFirstSignal;
        std::array<VkFence, Backend::MAX_FRAMES_IN_FLIGHT> inFlightFences;
        std::array<VkSemaphore, Backend::MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphore;

        uint32_t frameIndex = 0;
        Backend::Device& device;
    };
}
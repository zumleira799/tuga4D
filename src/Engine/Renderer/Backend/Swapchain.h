#pragma once
#include <Engine/Renderer/Backend/DeviceObject.h>
#include <array>
#include <vector>

namespace tuga4d::Engine {
    class Window;
}
namespace tuga4d::Engine::Renderer::Backend {
    constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    class Swapchain : public DeviceObject {
    public:
        struct Builder {
            Builder(Window& window);
            Builder& SetVSync(bool enabled);
            Builder& SetFormat(VkFormat swapchainFormat);
            Builder& SetOldSwapchain(Swapchain* swapchain);
            Swapchain* Build(Device& device, const std::string& debugName);
        private:
            VkSurfaceKHR windowSurface;
            VkExtent2D windowExtent;

            bool vsync = true;
            VkFormat preferredFormat = VK_FORMAT_B8G8R8A8_UNORM;
            Swapchain* oldSwapchain = nullptr;
        };

        Swapchain(Device& device, VkPresentModeKHR preferredPresentMode, VkSurfaceFormatKHR surfaceFormat, VkSurfaceKHR windowSurface,
            VkExtent2D windowExtent, const std::string& debugName, Swapchain* oldSwapchain);

        bool IsOk() const {
            return swapchain != VK_NULL_HANDLE;
        }
    private:
        void CreateSynchronization();
        void CreateSwapchain();
        void CreateImageViews();
        void CreateRenderPass();
    protected:
        ~Swapchain();
    private:
        VkSwapchainKHR swapchain;
        VkSurfaceKHR windowSurface;
        VkExtent2D windowExtent;
        VkPresentModeKHR presentMode;
        VkSurfaceFormatKHR surfaceFormat;

        std::array<VkFence, MAX_FRAMES_IN_FLIGHT> inFlightFences;
        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphore;
        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphore;
        std::vector<VkImageView> swapchainImageViews{};
        std::vector<VkRenderingAttachmentInfo> swapchainAttachments{};

        uint32_t imageCount;
        Swapchain* oldSwapchain = nullptr;
    };
}
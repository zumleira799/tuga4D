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

        Swapchain(Device& device, VkPresentModeKHR presentMode, VkSurfaceFormatKHR surfaceFormat, VkSurfaceKHR windowSurface,
            VkExtent2D windowExtent, const std::string& debugName, Swapchain* oldSwapchain);

        VkResult AcquireNextImage(uint32_t frameIndex);

        void BeginRendering();
        void EndRendering();

        bool IsOk() const {
            return swapchain != VK_NULL_HANDLE;
        }

        VkSwapchainKHR GetSwapchain() {
            return swapchain;
        }
        VkSemaphore GetCurrentWaitSemaphore() {
            return imageAvailableSemaphore[imageIndex];
        }
        VkExtent2D GetExtent() {
            return windowExtent;
        }
        uint32_t GetImageIndex() {
            return imageIndex;
        }
    private:
        void CreateSemaphores();
        void CreateSwapchain();
        void CreateImageViews();
        void CreateRenderPass();
    protected:
        ~Swapchain();
    private:
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkSurfaceKHR windowSurface;
        VkExtent2D windowExtent;
        VkPresentModeKHR presentMode;
        VkSurfaceFormatKHR surfaceFormat;

        std::vector<VkImageView> swapchainImageViews{};
        std::vector<VkRenderingAttachmentInfo> swapchainAttachments{};
        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphore{};

        uint32_t imageIndex = 0;
        uint32_t imageCount = 0;
        Swapchain* oldSwapchain = nullptr;
    };
}
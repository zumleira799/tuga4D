#pragma once
#include <Engine/Renderer/Backend/DeviceObject.h>


namespace tuga4d::Engine {
    class Window;
}
namespace tuga4d::Engine::Renderer::Backend {
    class Swapchain : public DeviceObject {
    public:
        struct Builder {
            Builder(Window& window);
            Builder& SetPresentMode(VkPresentModeKHR presentMode);
            Builder& SetFormat(VkFormat swapchainFormat);
            Builder& SetOldSwapchain(Swapchain* swapchain);
            Swapchain* Build(Device& device, const std::string& debugName);
        private:
            VkSurfaceKHR windowSurface;
            VkExtent2D windowExtent;

            VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
            VkFormat preferredFomat = VK_FORMAT_B8G8R8A8_UNORM;
            Swapchain* oldSwapchain = nullptr;
        };

        Swapchain(Device& device, VkPresentModeKHR preferredPresentMode, VkFormat preferredSwapchainFormat, VkSurfaceKHR windowSurface,
            VkExtent2D windowExtent, const std::string& debugName, Swapchain* oldSwapchain);

        bool IsOk() const {
            return swapchain != VK_NULL_HANDLE;
        }
    private:
        void CreateSwapchain();
    protected:
        ~Swapchain();
    private:
        VkSwapchainKHR swapchain;
        VkSurfaceKHR windowSurface;
        VkExtent2D windowExtent;
        VkPresentModeKHR presentMode;
        VkFormat swapchainFormat;

        Swapchain* oldSwapchain;
    };
}
#include "Swapchain.h"
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Window.h>
#include <Core/Logger.h>
#include <cassert>

namespace tuga4d::Engine::Renderer::Backend {
    Swapchain::Builder::Builder(Window& window) {
        windowSurface = window.GetSurface();
        windowExtent = {window.GetWidth(), window.GetHeight()};
    }
    Swapchain::Builder& Swapchain::Builder::SetPresentMode(VkPresentModeKHR presentMode) {
        preferredPresentMode = presentMode;
        return *this;
    }
    Swapchain::Builder& Swapchain::Builder::SetFormat(VkFormat swapchainFormat) {
        preferredFomat = swapchainFormat;
        return *this;
    }
    Swapchain::Builder& Swapchain::Builder::SetOldSwapchain(Swapchain* swapchain) {
        oldSwapchain = swapchain;
        return *this;
    }
    Swapchain* Swapchain::Builder::Build(Device& device, const std::string& debugName) {
        return new Swapchain(device, preferredPresentMode, preferredFomat, windowSurface, windowExtent,
            debugName, oldSwapchain);
    }

    Swapchain::Swapchain(Device& device, VkPresentModeKHR preferredPresentMode, VkFormat preferredSwapchainFormat, VkSurfaceKHR windowSurface,
        VkExtent2D windowExtent, const std::string& debugName, Swapchain* oldSwapchain)
        : DeviceObject(device), windowExtent(windowExtent), windowSurface(windowSurface), oldSwapchain(oldSwapchain) {
        CreateSwapchain();
        CreateDebugInfo(debugName, (uint64_t)swapchain, VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT);
    }
    Swapchain::~Swapchain() {
        vkDestroySwapchainKHR(device.GetDevice(), swapchain, nullptr);
    }

    void Swapchain::CreateSwapchain() {
        // FIXME
    }
}
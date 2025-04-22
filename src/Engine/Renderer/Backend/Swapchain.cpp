#include "Swapchain.h"
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Window.h>
#include <Core/Logger.h>
#include <cassert>

namespace tuga4d::Engine::Renderer::Backend {
    Swapchain::Builder::Builder(Window& window) {
        windowSurface = window.GetSurface();
        windowExtent = {window.GetWidth(), window.GetHeight()};
    }
    Swapchain::Builder& Swapchain::Builder::SetVSync(bool enabled) {
        vsync = enabled;
        return *this;
    }
    Swapchain::Builder& Swapchain::Builder::SetFormat(VkFormat swapchainFormat) {
        preferredFormat = swapchainFormat;
        return *this;
    }
    Swapchain::Builder& Swapchain::Builder::SetOldSwapchain(Swapchain* swapchain) {
        oldSwapchain = swapchain;
        return *this;
    }
    Swapchain* Swapchain::Builder::Build(Device& device, const std::string& debugName) {
        SwapchainSupportInfo supportInfo = device.GetSwapchainSupport(windowSurface);
		VkSurfaceFormatKHR finalFormat{};
		for (const auto& availableFormat : supportInfo.formats) {
			if (availableFormat.format == preferredFormat && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				finalFormat = availableFormat;
			}
		}
        if (finalFormat.format == VK_FORMAT_UNDEFINED) {
            Logger::Warning("Swapchain format %i is not available! Using fallback...", (int)preferredFormat);
            finalFormat = supportInfo.formats[0];
        }
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        if (vsync == false) {
			for (const auto& availablePresentMode : supportInfo.presentModes) {
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				} else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                    presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}
        return new Swapchain(device, presentMode, finalFormat, windowSurface, windowExtent,
            debugName, oldSwapchain);
    }

    Swapchain::Swapchain(Device& device, VkPresentModeKHR presentMode, VkSurfaceFormatKHR surfaceFormat, VkSurfaceKHR windowSurface,
        VkExtent2D windowExtent, const std::string& debugName, Swapchain* oldSwapchain)
        : DeviceObject(device), surfaceFormat(surfaceFormat), presentMode(presentMode), windowExtent(windowExtent),
        windowSurface(windowSurface), oldSwapchain(oldSwapchain) {        
        CreateSwapchain();
        CreateSemaphores();
        CreateImageViews();
        CreateRenderPass();
        
        CreateDebugInfo(debugName, (uint64_t)swapchain, VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT);
    }
    VkResult Swapchain::AcquireNextImage() {
        VkAcquireNextImageInfoKHR acquireInfo{ VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR };
        acquireInfo.swapchain = swapchain;
        acquireInfo.timeout = UINT64_MAX;
        acquireInfo.semaphore = imageAvailableSemaphore[imageIndex];
        acquireInfo.deviceMask = 0x1;
        acquireInfo.fence = VK_NULL_HANDLE;
        return vkAcquireNextImage2KHR(device.GetDevice(), &acquireInfo, &imageIndex);
    }
    void Swapchain::BeginRendering() {
        // FIXME
        // vkCmdBeginRendering();
    }
    void Swapchain::EndRendering() {
        // FIXME
        // vkCmdEndRendering();
    }
    Swapchain::~Swapchain() {
        for (int i = 0; i < imageCount; ++i) {
            vkDestroySemaphore(device.GetDevice(), imageAvailableSemaphore[i], nullptr);
            vkDestroyImageView(device.GetDevice(), swapchainImageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(device.GetDevice(), swapchain, nullptr);
    }
    void Swapchain::CreateSemaphores() {
        imageAvailableSemaphore.resize(imageCount);
        for (int i = 0; i < imageCount; ++i) {
            VkSemaphoreCreateInfo createInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            if (vkCreateSemaphore(device.GetDevice(), &createInfo, nullptr, &imageAvailableSemaphore[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create semaphore!");
            }
        }
    }
    void Swapchain::CreateSwapchain() {
        SwapchainSupportInfo supportInfo = device.GetSwapchainSupport(windowSurface);
        VkSwapchainCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = windowSurface;
        createInfo.minImageCount = MAX_FRAMES_IN_FLIGHT;
        createInfo.imageExtent = windowExtent;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = NULL;
        createInfo.preTransform = supportInfo.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        if(oldSwapchain != nullptr){
            createInfo.oldSwapchain = oldSwapchain->swapchain;
        }

        vkCreateSwapchainKHR(device.GetDevice(), &createInfo, NULL, &swapchain);
		vkGetSwapchainImagesKHR(device.GetDevice(), swapchain, &imageCount, nullptr);
    }
        
    void Swapchain::CreateImageViews() {
        std::vector<VkImage> swapchainImages{};
		swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device.GetDevice(), swapchain, &imageCount, swapchainImages.data());

        swapchainImageViews.resize(imageCount);
		for (int i = 0; i < imageCount; ++i) {
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = surfaceFormat.format;
			VkImageSubresourceRange subresourceRange{};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;
			imageViewCreateInfo.subresourceRange = subresourceRange;
			imageViewCreateInfo.image = swapchainImages[i];
			if (vkCreateImageView(device.GetDevice(), &imageViewCreateInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create image view!");
			}
		}
    }
    void Swapchain::CreateRenderPass() {
        for (int i = 0; i < imageCount; ++i) {
            VkRenderingAttachmentInfo attachmentInfo{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
            attachmentInfo.clearValue.color = { 0, 0, 0, 0 };
            attachmentInfo.imageView = swapchainImageViews[i];
            attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            attachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
            //attachmentInfo.
            swapchainAttachments.push_back(attachmentInfo);
        }
    }
}
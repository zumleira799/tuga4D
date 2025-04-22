#include "BaseApp.h"
#include <cassert>
#include <chrono>
#include <GLFW/glfw3.h>

#define MAX_FRAME_TIME 0.25F

using namespace tuga4d::Engine;
using namespace tuga4d::Engine::Renderer;
using namespace tuga4d::Engine::Renderer::Backend;
using namespace tuga4d::Engine::Input;
namespace tuga4d::Application {
    BaseApp::BaseApp() {
        Window::InitWindowManager();

        instance = new Instance(1, 0, 0, "Minha applicacao tuga");

        device = new Device(*instance, { VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
            VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME });
        renderQueue = new RenderQueue(*device);

        window = new Window(*instance, "grote ballen");
        swapchain = Swapchain::Builder(*window)
            .SetFormat(VK_FORMAT_B8G8R8A8_UNORM)
            .SetVSync(true)
            .Build(*device, "Main Swapchain");
    }

    BaseApp::~BaseApp() {
        device->DestroyObject(swapchain);
        vkDeviceWaitIdle(device->GetDevice());
        delete window;
        delete renderQueue;
        delete device;
        delete instance;
        Window::TerminateWindowManager();
    }

    void BaseApp::Run() {
        auto timeStampPreviousFrame = std::chrono::high_resolution_clock::now();
        while (bIsAppOpen) {
            auto timeStampCurrentFrame = std::chrono::high_resolution_clock::now();
            // frame time in SECONDS
            float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                timeStampCurrentFrame - timeStampPreviousFrame).count();
            timeStampPreviousFrame = timeStampCurrentFrame;

            deltaTime = std::min(deltaTime, MAX_FRAME_TIME);

            Window::PollEvents();
            if (window->isWindowClosed()) {
                bIsAppOpen = false;
            }

            OnFrame(deltaTime);

            renderQueue->WaitFences();
            if (VkResult acquireResult = swapchain->AcquireNextImage(renderQueue->GetFrameIndex()); acquireResult == VK_SUCCESS) {
                //renderQueue->SubmitCommand();
                renderQueue->PresentSwapchain(swapchain);
            } else {
                // resize logic
            }
            renderQueue->Flush();
        }
    }
}
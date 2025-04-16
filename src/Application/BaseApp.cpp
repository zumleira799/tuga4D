#include "BaseApp.h"
#include <cassert>
#include <chrono>
#include <GLFW/glfw3.h>

#define MAX_FRAME_TIME 0.25F

using namespace tuga4d::Engine::Input;
using namespace tuga4d::Engine;
using namespace tuga4d::Engine::Renderer::Backend;
namespace tuga4d::Application {
    BaseApp::BaseApp() {
        Window::InitWindowManager();

        instance = new Instance(1, 0, 0, "Minha applicacao tuga");
        device = new Device(*instance, { VK_KHR_SWAPCHAIN_EXTENSION_NAME });
        window = new Window(*instance, "grote ballen");
    }

    BaseApp::~BaseApp() {
        delete window;
        delete device;
        delete instance;

        Window::TerminateWindowManager();
    }

    void BaseApp::Run() {
        EventHandler<KeyEvent> handler = [](KeyEvent& e) {
            printf("%s\n", e.GetDebugInfo().c_str());
        };
        assert(window->BindEvent(handler));
        window->BindEvent<MouseScrollEvent>([&](MouseScrollEvent& e) {
            int posx, posy;
            glfwGetWindowPos(window->GetWindow(), &posx, &posy);
            glfwSetWindowPos(window->GetWindow(), posx + e.Horizontal, posy + e.Vertical);
        });

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
        }

        assert(window->UnbindEvent(handler));
    }
}
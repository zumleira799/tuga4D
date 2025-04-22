#pragma once
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Backend/Instance.h>
#include <Engine/Renderer/Backend/Swapchain.h>
#include <Engine/Renderer/RenderQueue.h>
#include <Engine/Window.h>

namespace tuga4d::Application {
    class BaseApp : NoCopy, NoMove {
    public:
        BaseApp();
        virtual ~BaseApp();

        void Run();

        void Stop() {
            bIsAppOpen = false;
        }
    protected:
        virtual void OnFrame(float deltaTime) {}
    private:
        bool bIsAppOpen = true;
        Engine::Renderer::Backend::Instance* instance;
        Engine::Renderer::Backend::Device* device;
        Engine::Window* window;
        Engine::Renderer::Backend::Swapchain* swapchain;
        Engine::Renderer::RenderQueue* renderQueue;
    };
}
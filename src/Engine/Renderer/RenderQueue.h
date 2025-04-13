#pragma once
#include <Engine/Renderer/Backend/Device.h>

namespace tuga4d::Engine::Renderer {
    class RenderQueue : NoCopy, NoMove {
    public:
        RenderQueue(Backend::Device& device);
        ~RenderQueue();
    private:
        Backend::Device& device;
    };
}
#pragma once
#include <Engine/Renderer/Backend/DeviceObject.h>

namespace tuga4d::Engine::Renderer::Backend {
    // TODO
    class Sampler : public DeviceObject {
    public:
        struct Builder {

        };

        bool IsOk() const {
            return sampler != VK_NULL_HANDLE;
        }
    private:
        VkSampler sampler;
    };
}
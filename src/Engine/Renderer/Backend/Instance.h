#pragma once
#include <Core/Util.h>
#include <string>
#include <vulkan/vulkan.h>

#ifndef NDEBUG
constexpr bool ENABLE_DEBUG_VALIDATION = false;
#else
constexpr bool ENABLE_DEBUG_VALIDATION = true;
#endif

namespace tuga4d::Engine::Renderer {
    inline namespace Backend {
        class Instance : NoCopy, NoMove {
        public:
            Instance(int versionMajor, int versionMinor, int versionPatch, const std::string& appName);

            VkInstance GetInstance() {
                return instance;
            }
        private:
            void CreateInstance(const std::string& appName);

            int vEngineMajor, vEngineMinor, vEnginePatch;

            VkInstance instance;
        };
    }
}
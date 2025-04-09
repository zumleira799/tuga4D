#pragma once
#include <Engine/Renderer/Backend/Instance.h>
#include <string>
#include <volk.h>

extern "C" {
    typedef struct GLFWwindow;
}
namespace tuga4d::Engine::Renderer {
    class Window : NoCopy, NoMove {
    public:
        static void InitWindowManager();

        Window(Backend::Instance& instance, const std::string& title,
            int width = 800, int height = 600, bool resizable = false);
        ~Window();

        VkSurfaceKHR GetSurface() {
            return surface;
        }
        GLFWwindow* GetWindow() {
            return window;
        }
    private:
        void CreateWindow(const std::string& windowName, int width, int height);
        void CreateCallbacks();
        void CreateSurface();

        VkSurfaceKHR surface;
        GLFWwindow* window;
        Backend::Instance& instance;
    };
}
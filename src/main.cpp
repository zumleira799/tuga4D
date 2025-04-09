#include <GLFW/glfw3.h>
#include <Engine/Renderer/Backend/Instance.h>
#include <Engine/Renderer/Backend/Device.h>
#include <Engine/Renderer/Window.h>

const std::vector<char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

using namespace tuga4d::Engine::Renderer::Backend;
using namespace tuga4d::Engine::Renderer;
int main() {
    Window::InitWindowManager();
    Instance* instance = new Instance(1, 0, 0, "Minha applicacao tuga");
    Window* window = new Window(*instance, "grote ballen");
    Device* device = new Device(*instance, deviceExtensions);
    while(!window->isWindowClosed()){
        glfwPollEvents();
    }
    delete device;
    delete window;
    delete instance;
    glfwTerminate();
    return EXIT_SUCCESS;
}
#include <GLFW/glfw3.h>
#include <Engine/Renderer/Backend/Instance.h>
#include <Engine/Renderer/Backend/Device.h>

const std::vector<char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

using namespace tuga4d::Engine::Renderer::Backend;
int main() {
    Instance* instance = new Instance(1, 0, 0, "Minha applicacao tuga");
    Device* device = new Device(*instance, deviceExtensions);
    delete device;
    delete instance;
    return EXIT_SUCCESS;
}
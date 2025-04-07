#include <GLFW/glfw3.h>
#include <Engine/Renderer/Backend/Instance.h>
#include <Engine/Renderer/Backend/Device.h>

using namespace tuga4d::Engine::Renderer::Backend;
int main() {
    Instance* instance = new Instance(1, 0, 0, "Minha applicacao tuga");
    Device* device = new Device(*instance);
    delete device;
    delete instance;
    return EXIT_SUCCESS;
}
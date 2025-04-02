#include <GLFW/glfw3.h>
#include <Engine/Renderer/Backend/Instance.h>

using namespace tuga4d::Engine;
int main() {
    Renderer::Instance* instance = new Renderer::Instance(1, 0, 0, "Minha applicacao tuga");
    delete instance;
    return EXIT_SUCCESS;
}
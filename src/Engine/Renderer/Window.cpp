#define GLFW_INCLUDE_VULKAN
#include "Window.h"
#include <Core/Logger.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

using namespace tuga4d::Engine::Renderer::Backend;
namespace tuga4d::Engine::Renderer {
    void Window::InitWindowManager() {
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("Failed to create GLFW instance!");
        }
        // disable legacy OpenGL functionality
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        Logger::Trace("Initalised GLFW");
    }
    Window::Window(Instance& instance, const std::string& windowName, int width, int height, bool resizable) 
    : instance(instance), width(width), height(height) {
        Logger::Trace("Creating window %p %s %i x %i", this, windowName.c_str(), width, height);

        glfwWindowHint(GLFW_RESIZABLE, resizable);
        CreateWindow(windowName, width, height);
        CreateCallbacks();
        CreateSurface();
    }

    Window::~Window() {
        Logger::Trace("Destroying window %p", this);
        vkDestroySurfaceKHR(instance.GetInstance(), surface, nullptr);
        glfwDestroyWindow(window);
    }

    bool Window::isWindowClosed(){
        return glfwWindowShouldClose(window);
    }
    VkExtent2D GetExtent() {
    }

    void Window::CreateWindow(const std::string& windowName, int width, int height) {
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
    }

    void Window::CreateCallbacks() {
        // Here you would implement your callbacks for resizing, input, etc.
    }

    void Window::CreateSurface() {
        VkResult result = glfwCreateWindowSurface(instance.GetInstance(), window, nullptr, &surface);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }
}
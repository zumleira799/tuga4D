#define GLFW_INCLUDE_VULKAN
#include "Window.h"
#include <Core/Logger.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

using namespace tuga4d::Engine::Input;
using namespace tuga4d::Engine::Renderer::Backend;
namespace tuga4d::Engine {
    void Window::InitWindowManager() {
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("Failed to create GLFW instance!");
        }
        // disable legacy OpenGL functionality
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        Logger::Trace("Initalised GLFW");
    }
    void Window::TerminateWindowManager() {
        Logger::Trace("Terminating GLFW");
        glfwTerminate();
    }
    void Window::PollEvents() {
        glfwPollEvents();
    }

    void Window::WaitEvents(double timeout) {
        timeout == -1.0 ? glfwWaitEvents() : glfwWaitEventsTimeout(timeout);
    }

    Window::Window(Instance& instance, const std::string& windowName, int width, int height, bool resizable) 
    : instance(instance), width(width), height(height), windowName(windowName) {
        Logger::Trace("Creating window %s %i x %i", windowName.c_str(), width, height);

        glfwWindowHint(GLFW_RESIZABLE, resizable);
        CreateWindow(windowName, width, height);
        CreateCallbacks();
        CreateSurface();
    }

    Window::~Window() {
        Logger::Trace("Destroying window %s", windowName.c_str());
        vkDestroySurfaceKHR(instance.GetInstance(), surface, nullptr);
        glfwDestroyWindow(window);
    }

    bool Window::isWindowClosed() const {
        return glfwWindowShouldClose(window);
    }

    void Window::SetSize(uint32_t width, uint32_t height) {
        glfwSetWindowSize(window, static_cast<int>(width), static_cast<int>(height));
    }

    void Window::SetTitle(const std::string& name) {
        windowName = name;
        glfwSetWindowTitle(window, name.c_str());
    }

    void Window::CreateWindow(const std::string& windowName, int width, int height) {
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
    }

    void Window::CreateCallbacks() {
        glfwSetKeyCallback(window, Window::KeyCallback);
        glfwSetMouseButtonCallback(window, Window::MouseButtonCallback);
        glfwSetScrollCallback(window, Window::MouseScrollCallback);
        glfwSetCursorPosCallback(window, Window::MousePositionCallback);
        glfwSetCharCallback(window, Window::CharCallback);
        glfwSetWindowSizeCallback(window, Window::ResizeCallback);
    }

    void Window::KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
        Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        KeyEvent event = KeyEvent(*pThis, static_cast<Key>(key), static_cast<Modifiers::Flags>(mods), 
            action == GLFW_PRESS || action == GLFW_REPEAT, action == GLFW_REPEAT);
        pThis->GetEventDispatcher<KeyEvent>().Dispatch(event);
    }
    void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        MouseButtonEvent event = MouseButtonEvent(*pThis, static_cast<MouseButton>(button), 
            static_cast<Modifiers::Flags>(mods), action == GLFW_PRESS);
        pThis->GetEventDispatcher<MouseButtonEvent>().Dispatch(event);
    }
    void Window::MouseScrollCallback(GLFWwindow* window, double x, double y) {
        Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        MouseScrollEvent event = MouseScrollEvent(*pThis, static_cast<float>(x),
            static_cast<float>(y));
        pThis->GetEventDispatcher<MouseScrollEvent>().Dispatch(event);
    }
    void Window::MousePositionCallback(GLFWwindow* window, double x, double y) {
        Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        MousePositionEvent event = MousePositionEvent(*pThis, static_cast<float>(x),
            static_cast<float>(y));
        pThis->GetEventDispatcher<MousePositionEvent>().Dispatch(event);
    }
    void Window::CharCallback(GLFWwindow* window, unsigned int codePoint) {
        Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        CharInputEvent event = CharInputEvent(*pThis, codePoint);
        pThis->GetEventDispatcher<CharInputEvent>().Dispatch(event);
    }
    void Window::ResizeCallback(GLFWwindow* window, int width, int height) {
        Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        pThis->width = width;
        pThis->height = height;
        WindowResizeEvent event = WindowResizeEvent(*pThis, width, height);
        pThis->GetEventDispatcher<WindowResizeEvent>().Dispatch(event);
    }

    void Window::CreateSurface() {
        VkResult result = glfwCreateWindowSurface(instance.GetInstance(), window, nullptr, &surface);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }
}
#pragma once
#include <Engine/Input/KeyEvent.h>
#include <Engine/Input/MouseButtonEvent.h>
#include <Engine/Input/MouseScrollEvent.h>
#include <Engine/Input/MousePositionEvent.h>
#include <Engine/Input/CharInputEvent.h>
#include <Engine/Input/WindowResizeEvent.h>

#include <Engine/Input/Dispatcher.h>

#include <Engine/Renderer/Backend/Instance.h>
#include <volk.h>

#include <array>
#include <variant>

extern "C" {
    typedef struct GLFWwindow;
}
namespace tuga4d::Engine {
    class Window : NoCopy, NoMove {
    public:
        using EventMap = std::array<std::variant<
            Input::EventDispatcher<Input::KeyEvent>,
            Input::EventDispatcher<Input::MouseButtonEvent>,
            Input::EventDispatcher<Input::MouseScrollEvent>,
            Input::EventDispatcher<Input::MousePositionEvent>,
            Input::EventDispatcher<Input::CharInputEvent>, 
            Input::EventDispatcher<Input::WindowResizeEvent>
        >, static_cast<size_t>(Input::EventType::COUNT)>;

        static void InitWindowManager();
        static void TerminateWindowManager();
        static void PollEvents();
        static void WaitEvents(double timeout);

        Window(Renderer::Backend::Instance& instance, const std::string& title,
            int width = 800, int height = 600, bool resizable = false);
        ~Window();

        bool isWindowClosed();

        uint32_t GetWidth() {
            return width;
        }
        uint32_t GetHeight() {
            return height;
        }

        VkSurfaceKHR GetSurface() {
            return surface;
        }
        GLFWwindow* GetWindow() {
            return window;
        }

        template <typename T>
        bool BindEvent(const Input::EventHandler<T>& handler) {
            return GetEventDispatcher<T>().Bind(handler);
        } 
        template <typename T>
        Input::EventHandler<T> BindEvent(std::function<void(T&)>&& fn) {
            Input::EventHandler<T> handler = Input::EventHandler<T>(std::move(fn));
            GetEventDispatcher<T>().Bind(handler);
            return handler;
        }
        template <typename T>
        bool UnbindEvent(const Input::EventHandler<T>& handler) {
            return GetEventDispatcher<T>().Unbind(handler);
        }

    private:
        static void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void MouseScrollCallback(GLFWwindow* window, double x, double y);
        static void MousePositionCallback(GLFWwindow* window, double x, double y);
        static void CharCallback(GLFWwindow* window, unsigned int codePoint);
        static void ResizeCallback(GLFWwindow* window, int width, int height);
        
        template <typename T>
        Input::EventDispatcher<T>& GetEventDispatcher() {
            return std::get<Input::EventDispatcher<T>>(events[static_cast<size_t>(T::Type)]);
        }

        EventMap events = {
           Input::EventDispatcher<Input::KeyEvent>(),
           Input::EventDispatcher<Input::MouseButtonEvent>(),
           Input::EventDispatcher<Input::MouseScrollEvent>(),
           Input::EventDispatcher<Input::MousePositionEvent>(),
           Input::EventDispatcher<Input::CharInputEvent>(),
           Input::EventDispatcher<Input::WindowResizeEvent>()
        };
    private:
        void CreateWindow(const std::string& windowName, int width, int height);
        void CreateCallbacks();
        void CreateSurface();

        VkSurfaceKHR surface;
        GLFWwindow* window;
        Renderer::Backend::Instance& instance;

        uint32_t width, height;
    };
}
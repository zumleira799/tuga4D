#pragma once
#include <Core/Util.h>
#include <string>

namespace tuga4d::Engine {
    class Window;
}
namespace tuga4d::Engine::Input {
    enum class EventType : int {
        Unknown = -1,
        Key,

        MouseButton,
        MouseScroll,
        MousePosition,

        CharInput,

        WindowResize,

        COUNT
    };

    template<enum EventType E = EventType::Unknown>
    class Event : NoCopy {
    public:
        static constexpr EventType Type = E;

        Event(Window& sender) 
        : window(&sender) {}
        virtual ~Event() = default;

        virtual EventType GetType() const = 0;
        virtual std::string GetName() const = 0;
        // Useful for logging
        virtual std::string GetDebugInfo() const = 0;

        Window& GetSender() {
            return *window;
        }
    protected:
        Window* window;
    };
}
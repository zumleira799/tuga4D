#pragma once
#include "./Event.h"

namespace tuga4d::Engine::Input {
    class WindowResizeEvent : public Event<EventType::WindowResize> {
    public:
        WindowResizeEvent(Window& sender, int width, int height)
            : Event(sender), Width(width), Height(height) {}

        virtual EventType GetType() const override {
            return EventType::CharInput;
        }
        virtual std::string GetName()  const override {
            return "CharInputEvent";
        }
        virtual std::string GetDebugInfo()  const override {
            return "Window Resize: (" + std::to_string(width) + ", " + std::to_string(height) + ")";
        }

        const int width;
        const int height;
    };
}
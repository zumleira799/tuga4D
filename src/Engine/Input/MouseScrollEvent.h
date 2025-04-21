#pragma once
#include "./Event.h"

namespace tuga4d::Engine::Input {
    class MouseScrollEvent : public Event<EventType::MouseScroll> {
    public:
        MouseScrollEvent(Window& sender, float x, float y)
            : Event(sender), Horizontal(x), Vertical(y) {}

        virtual EventType GetType() const override {
            return EventType::MouseScroll;
        }
        virtual std::string GetName()  const override {
            return "MouseScrollEvent";
        }
        virtual std::string GetDebugInfo()  const override {
            return "Mouse Scroll : (" + std::to_string(horizontal) + ", " + std::to_string(vertical) + ")";
        }

        const float horizontal;
        const float vertical;
    };
}
#pragma once
#include "./Event.h"

namespace tuga4d::Engine::Input {
    class MousePositionEvent : public Event<EventType::MousePosition> {
    public:
        MousePositionEvent(Window& sender, float x, float y)
            : Event(sender), PosX(x), PosY(y) {}

        virtual EventType GetType() const override {
            return EventType::MousePosition;
        }
        virtual std::string GetName()  const override {
            return "MousePositionEvent";
        }
        virtual std::string GetDebugInfo()  const override {
            return "Mouse Position : (" + std::to_string(posX) + ", " + std::to_string(posY) + ")";
        }

        const float posX;
        const float posY;
    };
}
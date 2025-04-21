#pragma once
#include "./Event.h"
#include "./Modifiers.h"
#include "./Buttons.h"

namespace tuga4d::Engine::Input {
    class MouseButtonEvent : public Event<EventType::MouseButton> {
    public:
        MouseButtonEvent(Window& sender, MouseButton btn, Modifiers::Flags mods, bool isDown)
            : Event(sender), button(btn), modifiers(mods), isDown(isDown) {}

        virtual EventType GetType() const override {
            return EventType::MouseButton;
        }
        virtual std::string GetName()  const override {
            return "MouseButtonEvent";
        }
        virtual std::string GetDebugInfo()  const override {
            return "Mouse Button : " + std::to_string((int)button) + " " + (isDown ? "Down" : "Up");
        }

        const MouseButton button;
        const Modifiers::Flags modifiers;
        const bool isDown;
    };
}
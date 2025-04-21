#pragma once
#include "./Event.h"
#include "./Modifiers.h"

namespace tuga4d::Engine::Input {
    class CharInputEvent : public Event<EventType::CharInput> {
    public:
        using UnicodeCharacter = int;
        CharInputEvent(Window& sender, int unicode)
        : Event(sender), Unicode(unicode) {}

        virtual EventType GetType() const override {
            return EventType::CharInput;
        }
        virtual std::string GetName()  const override {
            return "CharInputEvent";
        }
        virtual std::string GetDebugInfo()  const override {
            // TODO: Support for not just ASCII characters
            return "Char Typed: " + (char)Unicode;
        }

        const UnicodeCharacter unicode;
    };
}
#pragma once
#include "./Event.h"
#include "./Modifiers.h"
#include "./Keys.h"

namespace tuga4d::Engine::Input {
    class KeyEvent : public Event<EventType::Key> {
    public:
        KeyEvent(Window& sender, Key key, Modifiers::Flags mods, bool isDown, bool repeat)
            : Event(sender), key(key), modifiers(mods), isDown(isDown), isRepeating(repeat) {}

        virtual EventType GetType() const override {
            return EventType::Key;
        }
        virtual std::string GetName()  const override {
            return "KeyEvent";
        }
        virtual std::string GetDebugInfo()  const override {
            return "Key : " + std::to_string((int)key) + " " + 
                (isDown ? (isRepeating ? "Repeating" : "Down") : "Up");
        }

        const Key key;
        const Modifiers::Flags modifiers;
        const bool isDown;
        const bool isRepeating;
    };
}
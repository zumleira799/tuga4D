#pragma once
#include "./Event.h"
#include <vector>
#include <functional>

namespace tuga4d::Engine::Input {
    template <typename T>
    class EventHandler {
    public:
        using Functor = std::function<void(T&)>;

        EventHandler(const Functor& fn) : fn(fn) {}
        
        EventHandler(const EventHandler&) = default;
        EventHandler& operator=(const EventHandler&) = default;

        bool operator ==(const EventHandler& other) const {
            return this->handle == other.handle;
        }

        void operator()(T& e) {
            fn(e);
        }
    private:
        static inline int counter = 0;
        int handle = counter++;
        Functor fn;
    };

    template <typename T>
    class EventDispatcher {
    public:
        EventDispatcher() = default;

        void Dispatch(T& e) {
            for (EventHandler<T> fn : functions) {
                fn(e);
            }
        }
        bool Bind(const EventHandler<T>& handler) {
            if (std::find(functions.begin(), functions.end(), handler) != functions.end()) {
                return false;
            }
            functions.emplace_back(std::move(handler));
            return true;
        }
        bool Unbind(const EventHandler<T>& handler) {
            auto it = std::find(functions.begin(), functions.end(), handler);
            if (it == functions.end()) {
                return false;
            }
            functions.erase(it);
            return true;
        }
    private:
        std::vector<EventHandler<T>> functions;
    };
}
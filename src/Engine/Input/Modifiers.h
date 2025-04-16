#pragma once
namespace tuga4d::Engine::Input {
    struct Modifiers {
        using Flags = int;
        enum Bits : Flags {
            Shift       = 0x0001,
            Control     = 0x0002,
            Alt         = 0x0004,
            Super       = 0x0008,
            CapsLock    = 0x0010,
            NumLock     = 0x0020
        };
    };
}
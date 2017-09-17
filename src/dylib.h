#ifndef LLCE_DYLIB_H
#define LLCE_DYLIB_H

#include "keyboard.h"
#include "consts.h"

namespace llce {

struct memory {
    bool32_t isInitialized = false;

    uint64_t permanentSize = 0;
    void* permanent = nullptr;

    uint64_t transientSize = 0;
    void* transient = nullptr;
};

struct state {
    int32_t xpos = 0;
    int32_t ypos = 0;
};

struct input {
    bool8_t keys[keyboard::keycode::length] = { false, false, false, false, false };
};

}

#endif

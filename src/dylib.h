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
    const uint8_t lineLength = 5;
    int8_t line[5] = { 1, 2, 3, 4, 5 };
    uint8_t lineCursor = 0;
    uint8_t linePadding = 0;
    float64_t time = 0.0;
};


struct input {
    bool8_t keys[keyboard::keycode::length] = { false, false, false, false, false };
};

}

#endif

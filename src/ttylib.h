#ifndef LLCE_TTYLIB_H
#define LLCE_TTYLIB_H

#include "keyboard.h"
#include "consts.h"

namespace ttylib {

struct state {
    const uint8_t lineLength = 5;
    int8_t line[5] = { 1, 2, 3, 4, 5 };
    uint8_t lineCursor = 0;
    uint8_t linePadding = 0;
    float64_t time = 0.0;
};


struct input {
    bool8_t keys[llce::keyboard::keycode::length];
};

}

#endif

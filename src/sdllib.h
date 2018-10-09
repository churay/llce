#ifndef LLCE_SDLLIB_H
#define LLCE_SDLLIB_H

#include <SDL2/SDL.h>

#include "consts.h"

namespace sdllib {

struct state {
    float32_t box[4] = { 0.0, 0.0, 0.0, 0.0 }; // x, y, w, h
    float32_t boxColor[4] = { 0.0, 0.0, 0.0, 0.0 }; // r, g, b, a
    float32_t backColor[4] = { 1.0, 1.0, 1.0, 1.0 }; // r, g, b, a
    float64_t time = 0.0;
};


struct input {
    uint8_t keys[SDL_Scancode::SDL_NUM_SCANCODES];
};

}

#endif

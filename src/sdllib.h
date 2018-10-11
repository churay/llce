#ifndef LLCE_SDLLIB_H
#define LLCE_SDLLIB_H

#include <SDL2/SDL.h>

#include "consts.h"

namespace sdllib {

struct state {
    float32_t box[4] = { 0.0, 0.0, 0.0, 0.0 }; // x, y, w, h
    uint8_t boxColor[4] = { 0x00, 0x00, 0x00, 0xFF }; // r, g, b, a
    uint8_t backColor[4] = { 0xFF, 0xFF, 0xFF, 0xFF }; // r, g, b, a
    float64_t time = 0.0;
};


struct input {
    uint8_t keys[SDL_Scancode::SDL_NUM_SCANCODES];
};

}

#endif

#ifndef LLCE_SDLLIB_H
#define LLCE_SDLLIB_H

#include <SDL2/SDL.h>

#include "consts.h"

namespace sdllib {

struct state {
    uint32_t* texData = nullptr;
    int32_t texBox[4] = {0, 0, 0, 0}; // x, y, w, h
    void* texHandle = nullptr;
    bool8_t updated = false;
    float64_t time = 0.0;
};


struct input {
    uint8_t keys[SDL_Scancode::SDL_NUM_SCANCODES];
};

}

#endif

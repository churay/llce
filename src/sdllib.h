#ifndef LLCE_SDLLIB_H
#define LLCE_SDLLIB_H

#include "consts.h"

namespace sdllib {

struct state {
    float64_t time = 0.0;
};


struct input {
    bool8_t alive = false;
};

}

#endif

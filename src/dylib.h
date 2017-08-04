#ifndef LLCE_DYLIB_H
#define LLCE_DYLIB_H

#include "consts.h"

namespace llce {

struct memory {
    bool32_t initialized = false;

    uint64_t permanentSize = 0;
    void* permanent = nullptr;

    uint64_t transientSize = 0;
    void* transient = nullptr;
};

struct state {
    int32_t value = 0;
};

}

#endif

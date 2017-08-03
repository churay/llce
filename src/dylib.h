#ifndef LLCE_DYLIB_H
#define LLCE_DYLIB_H

#include "consts.h"

namespace llce {

struct memory {
    bool32_t isInitialized;

    uint64_t permanentSize;
    void* permanent;

    uint64_t transientSize;
    void* transient;
};

struct state {
    int32_t value;
};

}

#endif

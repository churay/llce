#include "dylib.h"

extern "C" void update( llce::state* pState ) {
    pState->xpos = pState->ypos = 1;
}

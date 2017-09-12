#include "dylib.h"

extern "C" void update( llce::state* pState ) {
    pState->value = 37;
}

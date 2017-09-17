#include <stdio.h>

#include "dylib.h"

extern "C" void update( llce::state* pState, llce::input* pInput ) {
    pState->xpos = pState->ypos = 2.0;
}

extern "C" void render( const llce::state* pState ) {
    printf( "Position @ [%f]: (%f, %f)  \r", pState->tt, pState->xpos, pState->ypos );
}

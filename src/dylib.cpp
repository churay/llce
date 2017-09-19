#include <stdio.h>

#include "dylib.h"

extern "C" void update( llce::state* pState, llce::input* pInput ) {
    pState->xpos = pState->ypos = 2.0;
}

extern "C" void render( const llce::state* pState, const llce::input* pInput ) {
    // printf( "Position @ [%f]: (%f, %f)  \r",
    //     pState->time, pState->xpos, pState->ypos );
    printf( "Inputs @ [%f]: H[%d], J[%d], K[%d], L[%d]\n",
        pState->time,
        pInput->keys[llce::keyboard::keycode::h],
        pInput->keys[llce::keyboard::keycode::j],
        pInput->keys[llce::keyboard::keycode::k],
        pInput->keys[llce::keyboard::keycode::l] );
}

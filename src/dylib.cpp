#include <stdio.h>

#include "dylib.h"

LLCE_DYLOAD_API void update( llce::state* pState, llce::input* pInput ) {
    // Handle Cursor Slide //

    if( pInput->keys[llce::keyboard::keycode::h] ) {
        pState->lineCursor = ( pState->lineCursor == 0 ) ?
            pState->lineLength - 1 : pState->lineCursor - 1;
    } if( pInput->keys[llce::keyboard::keycode::l] ) {
        pState->lineCursor = ( pState->lineCursor + 1 ) %
            pState->lineLength;
    }

    // Handle Entry Adjustment //

    if( pInput->keys[llce::keyboard::keycode::j] ) {
        pState->line[(pState->lineCursor + 2) % pState->lineLength] -= 1;
    } if( pInput->keys[llce::keyboard::keycode::k] ) {
        pState->line[(pState->lineCursor + 2) % pState->lineLength] += 1;
    }
}


LLCE_DYLOAD_API void render( const llce::state* pState, const llce::input* pInput ) {
    printf( "(%.3f) %d %d [%d] %d %d\r",
        pState->time,
        pState->line[(pState->lineCursor + 0) % pState->lineLength],
        pState->line[(pState->lineCursor + 1) % pState->lineLength],
        pState->line[(pState->lineCursor + 2) % pState->lineLength],
        pState->line[(pState->lineCursor + 3) % pState->lineLength],
        pState->line[(pState->lineCursor + 4) % pState->lineLength] );

    /* -- Input Debugging Code --
    printf( "Inputs @ [%f]: H[%d], J[%d], K[%d], L[%d]\n",
        pState->time,
        pInput->keys[llce::keyboard::keycode::h],
        pInput->keys[llce::keyboard::keycode::j],
        pInput->keys[llce::keyboard::keycode::k],
        pInput->keys[llce::keyboard::keycode::l] );
    */
}

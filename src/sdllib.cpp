#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "sdllib.h"

LLCE_DYLOAD_API void update( sdllib::state* pState, sdllib::input* pInput ) {
    if( pInput->keys[SDL_SCANCODE_W] ) {
        pState->box[1] += 0.1;
    } if( pInput->keys[SDL_SCANCODE_S] ) {
        pState->box[1] -= 0.1;
    } if( pInput->keys[SDL_SCANCODE_A] ) {
        pState->box[0] -= 0.1;
    } if( pInput->keys[SDL_SCANCODE_D] ) {
        pState->box[0] += 0.1;
    }
}


LLCE_DYLOAD_API void render( const sdllib::state* pState, const sdllib::input* pInput ) {
    glClearColor( pState->backColor[0], pState->backColor[1], pState->backColor[2], pState->backColor[3] );
    glClear( GL_COLOR_BUFFER_BIT );

    glBegin( GL_QUADS );
        glColor4f( pState->boxColor[0], pState->boxColor[1], pState->boxColor[2], pState->boxColor[3] );
        glVertex2f( pState->box[0] + 0.0 * pState->box[2], pState->box[1] + 0.0 * pState->box[3] );
        glVertex2f( pState->box[0] + 1.0 * pState->box[2], pState->box[1] + 0.0 * pState->box[3] );
        glVertex2f( pState->box[0] + 1.0 * pState->box[2], pState->box[1] + 1.0 * pState->box[3] );
        glVertex2f( pState->box[0] + 0.0 * pState->box[2], pState->box[1] + 1.0 * pState->box[3] );
    glEnd();
}

#include <SDL2/SDL.h>

#include "sdllib.h"

LLCE_DYLOAD_API void update( sdllib::state* pState, sdllib::input* pInput ) {
    // NOTE(JRC): The movement numbers seem inverted because the values
    // being changed are the offsets for the underlying texture.  By
    // doing a positive offset in Y for example, we offset the texture
    // downward on the screen, giving the appearance of the window
    // "camera" going upwards.
    int32_t newTexX = pState->texBox[0], newTexY = pState->texBox[1];
    if( pInput->keys[SDL_SCANCODE_W] ) {
        newTexY += 1;
    } if( pInput->keys[SDL_SCANCODE_S] ) {
        newTexY -= 1;
    } if( pInput->keys[SDL_SCANCODE_A] ) {
        newTexX += 1;
    } if( pInput->keys[SDL_SCANCODE_D] ) {
        newTexX -= 1;
    }

    pState->updated = newTexX != pState->texBox[0] || newTexY != pState->texBox[1];
    pState->texBox[0] = newTexX;
    pState->texBox[1] = newTexY;
    if( pState->updated ) {
        for( int32_t y = 0; y < pState->texBox[3]; y++ ) {
            for( int32_t x = 0; x < pState->texBox[2]; x++ ) {
                int32_t i = x + y * pState->texBox[2];
                pState->texData[i] = 0;
                pState->texData[i] |= (uint8_t)( 0x00 ) << 24;                  // r
                pState->texData[i] |= (uint8_t)( y + pState->texBox[1] ) << 16; // g
                pState->texData[i] |= (uint8_t)( x + pState->texBox[0] ) << 8;  // b
                pState->texData[i] |= (uint8_t)( 0xFF ) << 0;                   // a
            }
        }
    }
}


LLCE_DYLOAD_API void render( SDL_Renderer* pRenderer, const sdllib::state* pState, const sdllib::input* pInput ) {
    SDL_Texture* texHandle = (SDL_Texture*)pState->texHandle;

    SDL_UpdateTexture( texHandle, nullptr, pState->texData, sizeof(uint32_t) * pState->texBox[2] );
    SDL_RenderClear( pRenderer );
    SDL_RenderCopy( pRenderer, texHandle, nullptr, nullptr );
    SDL_RenderPresent( pRenderer );
}
